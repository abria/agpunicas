#include "EditorScene.h"
#include <fstream>
#include "json.hpp"
#include "View.h"
#include "Window.h"
#include "EditableObject.h"
#include "EditorUI.h"
#include "Game.h"
#include "GameScene.h"
#include "core_version.h"

using namespace agp;

EditorScene::EditorScene(GameScene* gameScene, EditorUI* ui)
	: UIScene(gameScene->rect(), gameScene->pixelUnitSize())
{
	_ui = ui;
	_gameScene = gameScene;
	_gameRect = _gameScene->view()->rect();
	_gameScene->displayGameSceneOnly(true);
	_currentCategory = 0;
	for (int i = 0; i < MAX_CATEGORIES; i++)
		_categories.push_back(strprintf("Category %d", i));
	_currentObject = nullptr;
	_draggedObject = nullptr;
	_snapGrid = true;
	_gridCellSize = 1;
	_currentCell = new EditableObject(this, RectF(0, 0, _gridCellSize, _gridCellSize, _rect.yUp), "", _currentCategory, _categories);
	_isPanning = false;
	_isDragging = false;
	_cameraZoomVel = 0.1f;
	_blocking = true;

	_view->setRect(_gameRect);
	float ar = Game::instance()->aspectRatio();
	if (ar)
		_view->setFixedAspectRatio(ar);

	generateGrid();
	fromJson();
	updateState(State::DEFAULT);
}

void EditorScene::generateGrid()
{
	for (auto& g : _grid)
		g->kill();
	_grid.clear();
	for (float x = _rect.pos.x; x < _rect.pos.x + _rect.size.x; x += _gridCellSize)
	{
		_grid.push_back(new RenderableObject(this, RectF(x, _rect.pos.y, _gridCellSize, _rect.size.y, _rect.yUp), nullptr));
		_grid.back()->setBorderColor(GRID_COLOR);
	}
	for (float y = _rect.pos.y; y < _rect.pos.y + _rect.size.y; y += _gridCellSize)
	{
		_grid.push_back(new RenderableObject(this, RectF(_rect.pos.x, y, _rect.size.x, _gridCellSize, _rect.yUp), nullptr));
		_grid.back()->setBorderColor(GRID_COLOR);
	}
}

void EditorScene::fromJson()
{
	std::ifstream f(std::string(SDL_GetBasePath()) + DEFAULT_SAVE_FILENAME);
	if (!f.is_open())
		return;

	nlohmann::json j = nlohmann::json::parse(f);

	_categories = j["categories"].get<std::vector<std::string>>();

	std::vector<nlohmann::json> jsonObjects = j["objects"].get<std::vector<nlohmann::json>>();
	for (auto& json : jsonObjects)
		_editObjects.push_back(new EditableObject(this, json, _categories));

	f.close();
}

void EditorScene::toJson()
{
	std::ofstream f(std::string(SDL_GetBasePath()) + DEFAULT_SAVE_FILENAME);
	if (!f.is_open())
		return;

	nlohmann::ordered_json j;
	j["core_version"] = agp::core::VERSION();

	j["categories"] = _categories;

	std::vector <nlohmann::ordered_json> jsonObjects;
	for (auto& obj : _editObjects)
		jsonObjects.push_back(obj->toJson());
	j["objects"] = jsonObjects;
	
	f << j.dump(3);
	f.close();
}

void EditorScene::updateState(State newState)
{
	_ui->clearHelpboxText();
	_ui->setCrossCursor(newState == State::CREATE);
	_currentCell->setVisible(newState == State::RENAME_CATEGORY || (newState == State::CREATE && !_currentObject));

	if(newState != State::RENAME_CATEGORY && newState != State::RENAME_OBJECT)
		SDL_StopTextInput();

	if (newState == State::DEFAULT)
	{
		_ui->setHelpboxText(1, "Mouse: [LEFT] select, [RIGHT] delete, [MIDDLE] pan/zoom");
		_ui->setHelpboxText(0, "Keys: [C]reate, [G]rid on/off, [Q]uit/save");
		
		if (_currentObject)
			_currentObject->setSelected(false);
		_currentObject = nullptr;
	}
	else if (newState == State::CREATE)
	{
		_ui->setHelpboxText(1, "Mouse: [LEFT] start/end drawing");
		_ui->setHelpboxText(0, "Keys: [SPACE] change, [R]ename, [ESC]ape");
	}
	else if (newState == State::RENAME_CATEGORY)
	{
		_ui->setHelpboxText(1, strprintf("Rename to \"%s|\"", _textInput.c_str()));
		_ui->setHelpboxText(0, "Keys: [ENTER] accept, [ESC]ape");
	}
	else if (newState == State::RENAME_OBJECT)
	{
		_ui->setHelpboxText(1, strprintf("Rename to \"%s|\"", _textInput.c_str()));
		_ui->setHelpboxText(0, "Keys: [ENTER] accept, [ESC]ape");
	}
	else if (newState == State::SELECT)
	{
		_ui->setHelpboxText(1, "Mouse: [LEFT] drag/drop");
		_ui->setHelpboxText(0, "Keys: [R]ename, [ESC]ape");
		_currentObject->setSelected(true);
	}

	_state = newState;
}

void EditorScene::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	// panning
	if (_panningDelta.x != 0 || _panningDelta.y != 0)
	{
		_view->move(-_panningDelta / _view->magf());
		_panningDelta = PointF(0, 0);
	}

	// update cursor text
	if (_state == State::CREATE)
	{
		if(_snapGrid)
			_ui->setCursorText(strprintf("%.0f,%.0f", _mouseCoordsSnap.x, _mouseCoordsSnap.y));
		else
			_ui->setCursorText(strprintf("%.1f,%.1f", _mouseCoordsF.x, _mouseCoordsF.y));
	}
	else
		_ui->setCursorText("");

	// sync view with game scene's view
	_gameScene->view()->setRect(_view->rect());
}

void EditorScene::event(SDL_Event& evt)
{
	UIScene::event(evt);

	// detect CTRL and SHIFT modifiers
	const Uint8* keyboardState = SDL_GetKeyboardState(0);
	bool ctrlPressed = keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL];
	bool shiftPressed = keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];

	// reset focused states
	for (auto& editable : _editObjects)
		editable->setFocused(false);

	// text inputting
	if (_state == State::RENAME_CATEGORY || _state == State::RENAME_OBJECT)
	{
		if (evt.type == SDL_TEXTINPUT)
			_textInput += evt.text.text;
		else if (evt.type == SDL_KEYDOWN) 
		{
			if (evt.key.keysym.scancode == SDL_SCANCODE_BACKSPACE && !_textInput.empty())
				_textInput.pop_back();
			else if (evt.key.keysym.scancode == SDL_SCANCODE_RETURN)
			{
				if (_state == State::RENAME_CATEGORY)
				{
					_categories[_currentCategory] = _textInput;
					for (auto& editObj : _editObjects)
						editObj->setCategory(editObj->category());
					_currentCell->setCategory(_currentCategory);
					_state = State::CREATE;
				}
				else if (_state == State::RENAME_OBJECT)
				{
					_currentObject->setName(_textInput);
					_state = State::SELECT;
				}
			}
			else if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				_state = _state == State::RENAME_CATEGORY ? _state = State::CREATE : _state = State::SELECT;
		}
		updateState(_state);
		return;
	}

	// key events
	if (evt.type == SDL_KEYDOWN && !evt.key.repeat)
	{
		if (evt.key.keysym.scancode == SDL_SCANCODE_S)
			toggleSnapGrid();
		else if (_state == State::DEFAULT && evt.key.keysym.scancode == SDL_SCANCODE_C)
			updateState(State::CREATE);
		else if (_state == State::DEFAULT && evt.key.keysym.scancode == SDL_SCANCODE_Q)
		{
			toJson();
			_gameScene->view()->setRect(_gameRect);
			_gameScene->displayGameSceneOnly(false);
			Game::instance()->popSceneLater();	// _ui scene
			Game::instance()->popSceneLater();	// this scene
		}
		else if (_state == State::CREATE && evt.key.keysym.scancode == SDL_SCANCODE_SPACE)
		{
			_currentCategory = (_currentCategory + 1) % MAX_CATEGORIES;
			if (_currentObject)
				_currentObject->setCategory(_currentCategory);
			_currentCell->setCategory(_currentCategory);
		}
		else if (_state == State::CREATE && evt.key.keysym.scancode == SDL_SCANCODE_R)
		{
			updateState(State::RENAME_CATEGORY);
			_textInput = _categories[_currentCategory];
			SDL_StartTextInput();
			SDL_FlushEvent(SDL_TEXTINPUT);     // flush pending text input events
		}
		else if (_state == State::SELECT && evt.key.keysym.scancode == SDL_SCANCODE_R)
		{
			updateState(State::RENAME_OBJECT);
			_textInput = _currentObject->editName();
			SDL_StartTextInput();
			SDL_FlushEvent(SDL_TEXTINPUT);     // flush pending text input events
		}
		else if (evt.key.keysym.scancode == SDL_SCANCODE_G)
			toggleGrid();
		else if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			updateState(State::DEFAULT);
	}

	// mouse motion
	if (evt.type == SDL_MOUSEMOTION)
	{
		// mouse tracking
		_mouseCoordsF = PointF(float(evt.button.x), float(evt.button.y));
		_mouseCoordsF = _view->mapToScene(_mouseCoordsF);
		_mouseCoordsSnap.x = floor(_mouseCoordsF.x / _gridCellSize) * _gridCellSize;
		_mouseCoordsSnap.y = floor(_mouseCoordsF.y / _gridCellSize) * _gridCellSize;
		_mousePointCurr = _snapGrid ? _mouseCoordsSnap : _mouseCoordsF;

		// panning
		if (_isPanning)
		{
			PointF delta = PointF(float(evt.button.x), float(evt.button.y)) - _lastMousePositionPanning;
			_panningDelta += delta;
			_lastMousePositionPanning = PointF(float(evt.button.x), float(evt.button.y));
		}

		// drag
		if (!_isDragging)
		{
			float dragThreshold = _gridCellSize; // 1 grid unit
			float distance = (_mouseCoordsF - _dragStartMousePosition).mag();

			if (distance > dragThreshold && _draggedObject)
			{
				_isDragging = true;
				_dragStartMousePosition = _mouseCoordsF;
				_dragStartObjectPosition = _draggedObject->pos();
			}
		}
		// drop
		else if (_draggedObject)
		{
			PointF delta = _mouseCoordsF - _dragStartMousePosition;
			PointF newPos = _dragStartObjectPosition + delta;

			if (_snapGrid)
			{
				newPos.x = roundf(newPos.x / _gridCellSize) * _gridCellSize;
				newPos.y = roundf(newPos.y / _gridCellSize) * _gridCellSize;
			}

			if (_draggedObject->pos() != newPos)
				_draggedObject->setPos(newPos);
		}

		// object resizing
		if (_state == State::CREATE)
		{
			_currentCell->setPos(_mousePointCurr);

			if (_currentObject)
				_currentObject->setSize(_mousePointCurr - _currentObject->rect().pos + (_snapGrid ? PointF(_gridCellSize, _gridCellSize) : PointF(0, 0)));
		}

		// object focus
		else if (_state == State::DEFAULT)
		{
			EditableObject* underMouse = editableUnderMouse();
			if (underMouse)
				underMouse->setFocused(true);
		}
	}

	// mouse buttons
	if (evt.type == SDL_MOUSEWHEEL)
	{
		// grid
		if (ctrlPressed)
		{
			if (evt.wheel.y > 0)
				_gridCellSize *= 2;
			else if (evt.wheel.y < 0)
				_gridCellSize /= 2;
			generateGrid();
			_currentCell->setSize(PointF(_gridCellSize, _gridCellSize));
		}
		// zooom
		else
		{
			if (evt.wheel.y > 0)
				_view->scale(1 - _cameraZoomVel);
			else if (evt.wheel.y < 0)
				_view->scale(1 + _cameraZoomVel);
		}
	}
	else if (evt.button.type == SDL_MOUSEBUTTONDOWN)
	{
		if (_state == State::CREATE && evt.button.button == SDL_BUTTON_LEFT)
		{
			if (_currentObject)
			{
				_currentObject = nullptr;
				_currentCell->setVisible(true);
			}
			else
			{
				_currentCell->setVisible(false);
				_currentObject = new EditableObject(this, RectF(_mousePointCurr.x, _mousePointCurr.y, _gridCellSize, _gridCellSize, _rect.yUp), "Unnamed", _currentCategory, _categories);
				_editObjects.push_back(_currentObject);
			}
		}
		else if (_state == State::DEFAULT || _state == State::SELECT)
		{
			EditableObject* underMouse = editableUnderMouse();

			if (underMouse && evt.button.button == SDL_BUTTON_RIGHT)
			{
				_editObjects.erase(std::remove(_editObjects.begin(), _editObjects.end(), underMouse), _editObjects.end());
				killObject(underMouse);
			}
			else if (underMouse && evt.button.button == SDL_BUTTON_LEFT)
			{
				if(_currentObject)
					_currentObject->setSelected(false);
				_currentObject = underMouse;
				_currentObject->setSelected(true);
				updateState(State::SELECT);

				_dragStartMousePosition = _mouseCoordsF;
				_draggedObject = _currentObject;
			}
			else if (evt.button.button == SDL_BUTTON_LEFT)
				updateState(State::DEFAULT);
		}

		if (evt.button.button == SDL_BUTTON_MIDDLE)
		{
			_isPanning = true;
			_lastMousePositionPanning = PointF(float(evt.button.x), float(evt.button.y));
		}
	}
	else if (evt.type == SDL_MOUSEBUTTONUP)
	{
		if (evt.button.button == SDL_BUTTON_MIDDLE)
			_isPanning = false;
		else if (_draggedObject && evt.button.button == SDL_BUTTON_LEFT)
		{
			_isDragging = false;
			_draggedObject = nullptr;
		}
	}
}

EditableObject* EditorScene::editableUnderMouse()
{
	auto objectsUnderMouse = objects(_mouseCoordsF);
	std::list<EditableObject*> objectsVisible;
	for (auto obj : objectsUnderMouse)
		if (obj->to<EditableObject*>())
			objectsVisible.push_back(obj->to<EditableObject*>());
	if (objectsVisible.size())
		return objectsVisible.front();
	else
		return nullptr;
}

void EditorScene::toggleGrid()
{
	for (auto& g : _grid)
		g->setVisible(!g->visible());
}
