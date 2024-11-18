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
	_gameScene = gameScene;
	_ui = ui;
	_gameRect = _gameScene->view()->rect();
	_gameScene->displayGameSceneOnly(true);
	_currentCategory = 0;
	_currentObject = nullptr;
	_snapGrid = true;
	_blocking = true;
	_cameraZoomVel = 0.1f;
	_cameraTranslateVel = { 500, 500 };
	for (int i = 0; i < MAX_CATEGORIES; i++)
		_categories.push_back(strprintf("Category %d", i));

	_currentCell = new EditableObject(this, RectF(0, 0, 1, 1), "", _currentCategory, _categories);

	_view = new View(this, gameScene->view()->rect());
	float ar = Game::instance()->aspectRatio();
	if (ar)
		_view->setFixedAspectRatio(ar);

	fromJson();

	updateState(State::DEFAULT);
}

void EditorScene::fromJson()
{
	// generate grid
	for (float x = _rect.pos.x; x < _rect.pos.x + _rect.size.x; x++)
	{
		_grid.push_back(new RenderableObject(this, RectF(x, _rect.pos.y, 1, _rect.size.y, _rect.yUp), nullptr));
		_grid.back()->setBorderColor(GRID_COLOR);
	}
	for (float y = _rect.pos.y; y < _rect.pos.y + _rect.size.y; y++)
	{
		_grid.push_back(new RenderableObject(this, RectF(_rect.pos.x, y, _rect.size.x, 1, _rect.yUp), nullptr));
		_grid.back()->setBorderColor(GRID_COLOR);
	}

	// load objects from json file
	std::ifstream f("leveleditor.json");
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
	std::ofstream f("leveleditor.json");
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

	if (newState == State::DEFAULT)
	{
		_ui->setHelpboxText(1, "Mouse: [LEFT] select, [RIGHT] delete");
		_ui->setHelpboxText(0, "Keys: [C]reate, [G]rid on/off, [Q]uit/save");
		_ui->setCrossCursor(false);
		_currentCell->setVisible(false);
		if (_currentObject)
			_currentObject->setSelected(false);
		_currentObject = nullptr;
		SDL_StopTextInput();
	}
	else if (newState == State::CREATE)
	{
		//_ui->setHelpboxText(2, strprintf("Category: %s.", _categories[_currentCategory].c_str()));
		_ui->setHelpboxText(1, "Mouse: [LEFT] start/end drawing");
		_ui->setHelpboxText(0, "Keys: [SPACE] change, [R]ename, [ESC]ape");
		_ui->setCrossCursor(true);
		if(!_currentObject)
			_currentCell->setVisible(true);
		SDL_StopTextInput();
	}
	else if (newState == State::RENAME_CATEGORY)
	{
		_ui->setHelpboxText(1, strprintf("Rename to \"%s|\"", _textInput.c_str()));
		_ui->setHelpboxText(0, "Keys: [ENTER] accept, [ESC]ape");
		_ui->setCrossCursor(false);
		_currentCell->setVisible(true);
	}
	else if (newState == State::RENAME_OBJECT)
	{
		_ui->setHelpboxText(1, strprintf("Rename to \"%s|\"", _textInput.c_str()));
		_ui->setHelpboxText(0, "Keys: [ENTER] accept, [ESC]ape");
		_ui->setCrossCursor(false);
		_currentCell->setVisible(false);
	}
	else if (newState == State::SELECT)
	{
		//_ui->setHelpboxText(2, strprintf("\"%s\" selected, category: \"%s\"", _currentObject->editName().c_str(), _categories[_currentObject->category()].c_str()));
		_ui->setHelpboxText(1, "Mouse: [SCROLL] rotate");
		_ui->setHelpboxText(0, "Keys: [R]ename, [ESC]ape");
		_ui->setCrossCursor(false);
		_currentCell->setVisible(false);
		_currentObject->setSelected(true);
		SDL_StopTextInput();
	}

	_state = newState;
}

void EditorScene::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	const Uint8* keyboard = SDL_GetKeyboardState(0);

	Direction xDir = Direction::NONE;
	Direction yDir = Direction::NONE;

	if (keyboard[SDL_SCANCODE_RIGHT] && !keyboard[SDL_SCANCODE_LEFT])
		xDir = Direction::RIGHT;
	if (keyboard[SDL_SCANCODE_LEFT] && !keyboard[SDL_SCANCODE_RIGHT])
		xDir = Direction::LEFT;
	if (keyboard[SDL_SCANCODE_UP] && !keyboard[SDL_SCANCODE_DOWN])
		yDir = Direction::UP;
	if (keyboard[SDL_SCANCODE_DOWN] && !keyboard[SDL_SCANCODE_UP])
		yDir = Direction::DOWN;

	_view->move((_cameraTranslateVel / _view->magf()) * dir2vec(xDir, _rect.yUp) * timeToSimulate);
	_view->move((_cameraTranslateVel / _view->magf()) * dir2vec(yDir, _rect.yUp) * timeToSimulate);

	_gameScene->view()->setRect(_view->rect());

	if (_state == State::CREATE)
	{
		if(_snapGrid)
			_ui->setCursorText(strprintf("%.0f,%.0f", _mouseCoordsSnap.x, _mouseCoordsSnap.y));
		else
			_ui->setCursorText(strprintf("%.1f,%.1f", _mouseCoordsF.x, _mouseCoordsF.y));
	}
	else
		_ui->setCursorText("");
}

void EditorScene::event(SDL_Event& evt)
{
	UIScene::event(evt);

	// reset focused states
	for (auto& editable : _editObjects)
		editable->setFocused(false);

	// process text input first
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
				}
				else
					_currentObject->setName(_textInput);
				_state = State::DEFAULT;
			}
			else if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				_state = State::DEFAULT;
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
			Game::instance()->popSceneLater();
			Game::instance()->popSceneLater();
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
			SDL_StartTextInput();              // Start text input
			SDL_FlushEvent(SDL_TEXTINPUT);     // Flush pending text input events
		}
		else if (_state == State::SELECT && evt.key.keysym.scancode == SDL_SCANCODE_R)
		{
			updateState(State::RENAME_OBJECT);
			_textInput = _currentObject->editName();
			SDL_StartTextInput();              // Start text input
			SDL_FlushEvent(SDL_TEXTINPUT);     // Flush pending text input events
		}
		else if (evt.key.keysym.scancode == SDL_SCANCODE_G)
			toggleGrid();
		else if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			updateState(State::DEFAULT);
	}

	// update mouse coords
	if (evt.type == SDL_MOUSEMOTION)
	{
		_mouseCoordsF = PointF(float(evt.button.x), float(evt.button.y));
		_mouseCoordsF = _view->mapToScene(_mouseCoordsF);
		_mouseCoordsSnap = Point(int(_mouseCoordsF.x) + (_mouseCoordsF.x < 0 ? -1 : 0), int(_mouseCoordsF.y) + (_mouseCoordsF.y < 0 ? -1 : 0));
	}
	PointF mousePoint = _snapGrid ? _mouseCoordsSnap : _mouseCoordsF;

	// mouse events
	if (evt.type == SDL_MOUSEWHEEL)
	{
		if (evt.wheel.y > 0)
			_view->scale(1 - _cameraZoomVel);
		else if (evt.wheel.y < 0)
			_view->scale(1 + _cameraZoomVel);
	}
	else if (_state == State::CREATE && evt.type == SDL_MOUSEMOTION)
	{
		_currentCell->setPos(mousePoint);

		if (_currentObject)
			_currentObject->setSize(mousePoint - _currentObject->rect().pos + (_snapGrid ? PointF(1, 1) : PointF(0, 0)));
	}
	else if (_state == State::DEFAULT && evt.type == SDL_MOUSEMOTION)
	{
		EditableObject* underMouse = editableUnderMouse();
		if (underMouse)
			underMouse->setFocused(true);
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
				_currentObject = new EditableObject(this, RectF(mousePoint.x, mousePoint.y, 1, 1), "Unnamed", _currentCategory, _categories);
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
			}
			else if (evt.button.button == SDL_BUTTON_LEFT)
				updateState(State::DEFAULT);
		}
	}
}

EditableObject* EditorScene::editableUnderMouse()
{
	auto& objectsUnderMouse = objects(_mouseCoordsF);
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
