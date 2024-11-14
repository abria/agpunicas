#include "EditorScene.h"
#include <fstream>
#include "json.hpp"
#include "View.h"
#include "Window.h"
#include "EditableObject.h"
#include "Game.h"
#include "GameScene.h"
#include "core_version.h"

using namespace agp;

EditorScene::EditorScene(GameScene* gameScene)
	: UIScene(gameScene->rect(), gameScene->pixelUnitSize())
{
	_gameScene = gameScene;
	_gameRect = _gameScene->view()->rect();
	_gameScene->hideOverlayScenes(true);
	_currentCell = nullptr;
	_currentObject = nullptr;
	_snapGrid = true;
	_currentCategory = 0;
	_blocking = true;
	_cameraZoomVel = 0.1f;
	_cameraTranslateVel = { 500, 500 };
	for (int i = 0; i < MAX_CATEGORIES; i++)
		_categories.push_back(strprintf("Category %d", i));

	_view = new View(this, gameScene->view()->rect());
	float ar = Game::instance()->aspectRatio();
	if (ar)
		_view->setFixedAspectRatio(ar);

	fromJson();
}

void EditorScene::fromJson()
{
	// generate grid
	for (float x = _rect.pos.x; x < _rect.pos.x + _rect.size.x; x++)
	{
		_grid.push_back(new RenderableObject(this, RectF(x, _rect.pos.y, 1, _rect.size.y, _rect.yUp), nullptr));
		_grid.back()->setBorderColor(Color(100, 100, 100));
	}
	for (float y = _rect.pos.y; y < _rect.pos.y + _rect.size.y; y++)
	{
		_grid.push_back(new RenderableObject(this, RectF(_rect.pos.x, y, _rect.size.x, 1, _rect.yUp), nullptr));
		_grid.back()->setBorderColor(Color(100, 100, 100));
	}

	// load objects from json file
	std::ifstream f("leveleditor.json");
	if (!f.is_open())
		return;
	nlohmann::json j = nlohmann::json::parse(f);
	_categories = j["categories"].get<std::vector<std::string>>();
	std::vector<nlohmann::json> jsonObjects = j["objects"].get<std::vector<nlohmann::json>>();
	for (auto& json : jsonObjects)
		_editObjects.push_back(new EditableObject(this, json));
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
}

void EditorScene::event(SDL_Event& evt)
{
	UIScene::event(evt);

	PointF mousePoint(float(evt.button.x), float(evt.button.y));
	mousePoint = _view->mapToScene(mousePoint);
	if (_snapGrid)
		mousePoint = Point(int(mousePoint.x), int(mousePoint.y));

	// keyboard
	if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_SPACE && !evt.key.repeat && !_currentObject)
	{
		_currentCategory = (_currentCategory++) % MAX_CATEGORIES;
		if (_currentCell)
			_currentCell->setCategory(_currentCategory);
	}
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_S && !evt.key.repeat)
		toggleSnapGrid();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_Q && !evt.key.repeat)
	{
		toJson();
		Game::instance()->popScene();
		_gameScene->view()->setRect(_gameRect);
		_gameScene->hideOverlayScenes(false);
	}

	// mouse
	if (evt.type == SDL_MOUSEWHEEL)
	{
		if (evt.wheel.y > 0)
			_view->scale(1 - _cameraZoomVel);
		else if (evt.wheel.y < 0)
			_view->scale(1 + _cameraZoomVel);
	}
	else if (evt.type == SDL_MOUSEMOTION)
	{
		if (_currentCell)
			_currentCell->setPos(mousePoint);

		else if (!_currentObject)
			_currentCell = new EditableObject(this, RectF(mousePoint.x, mousePoint.y, 1, 1), _currentCategory);

		else if (_currentObject)
			_currentObject->setSize(mousePoint - _currentObject->rect().pos + PointF(1, 1));
	}
	else if (evt.button.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT)
	{
		if (_currentObject)
			_currentObject = nullptr;
		else
		{
			killObject(_currentCell);
			_currentCell = nullptr;
			_currentObject = new EditableObject(this, RectF(mousePoint.x, mousePoint.y, 1, 1), _currentCategory);
			_editObjects.push_back(_currentObject);
		}
	}
	//else if (evt.button.button == SDL_BUTTON_RIGHT)
		//new Slime(this, { mousePoint.x, mousePoint.y });
}
