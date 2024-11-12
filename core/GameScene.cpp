// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "GameScene.h"
#include "RenderableObject.h"
#include "EditableObject.h"
#include "View.h"
#include "Game.h"
#include "Audio.h"
#include "OverlayScene.h"
#include <fstream>
#include "json.hpp"

using namespace agp;

GameScene::GameScene(const RectF& rect, const Point& pixelUnitSize, float dt)
	: Scene(rect, pixelUnitSize)
{
	_dt = dt;
	_timeToSimulateAccum = 0;
	_player = nullptr;
	_cameraZoomVel = 0.1f;
	_cameraTranslateVel = { 500, 500 };
	_collidersVisible = false;
	_cameraManual = false;
	_cameraFollowsPlayer = true;
	_editMode = false;
	_editCurrentCell = nullptr;
	_editCurrentObject = nullptr;
	_snapOnGrid = true;
	_editCurrentCategory = 0;

	_view = new View(this, _rect);
	float ar = Game::instance()->aspectRatio();
	if(ar)
		_view->setFixedAspectRatio(ar);
}

void GameScene::render()
{
	if (_active)
	{
		for (auto& bgScene : _backgroundScenes)
			bgScene->render();

		_view->render();

		for (auto& fgScene : _foregroundScenes)
			fgScene->render();
	}
}

void GameScene::update(float timeToSimulate)
{
	Scene::update(timeToSimulate);

	if (!_active)
		return;

	updateOverlayScenes(timeToSimulate);
	updateControls(timeToSimulate);
	updateWorld(timeToSimulate);
	updateCamera(timeToSimulate);
}

void GameScene::updateOverlayScenes(float timeToSimulate)
{
	for (auto& bgScene : _backgroundScenes)
		bgScene->update(timeToSimulate);
	for (auto& fgScene : _foregroundScenes)
		fgScene->update(timeToSimulate);
}


void GameScene::updateControls(float timeToSimulate)
{
	// empty
}

void GameScene::updateWorld(float timeToSimulate)
{
	// semi-fixed timestep
	_timeToSimulateAccum += timeToSimulate;
	while (_timeToSimulateAccum >= _dt)
	{
		for (auto& layer : _sortedObjects)
			for (auto& obj : layer.second)
				if (!obj->freezed())
					obj->update(_dt);		// physics, collision, logic, animation

		_timeToSimulateAccum -= _dt;
	}
}

void GameScene::updateCamera(float timeToSimulate)
{
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

	if (_cameraManual)
	{
		_view->move((_cameraTranslateVel / _view->magf()) * dir2vec(xDir, _rect.yUp) * timeToSimulate);
		_view->move((_cameraTranslateVel / _view->magf()) * dir2vec(yDir, _rect.yUp) * timeToSimulate);
	}
	else if(_cameraFollowsPlayer)
	{
		_view->setX(_player->rect().pos.x - _view->rect().size.x / 2);
		_view->setY(_player->rect().pos.y - _view->rect().size.y / 2);
	}
}

void GameScene::event(SDL_Event& evt)
{
	Scene::event(evt);

	// window resize events may affect overlay scenes
	if (evt.type == SDL_WINDOWEVENT)
	{
		for (auto& bgScene : _backgroundScenes)
			bgScene->event(evt);
		for (auto& fgScene : _foregroundScenes)
			fgScene->event(evt);
	}

	// visual controls
	if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_C && !evt.key.repeat)
		toggleColliders();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_R && !evt.key.repeat)
		toggleRects();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_M && !evt.key.repeat)
		toggleCameraManual();
	else if (evt.type == SDL_MOUSEWHEEL)
	{
		if (evt.wheel.y > 0)
			_view->scale(1 - _cameraZoomVel);
		else if (evt.wheel.y < 0)
			_view->scale(1 + _cameraZoomVel);
	}

	// editor
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_E && !evt.key.repeat)
		toggleEditMode();
	else if (_editMode)
	{
		PointF mousePoint(float(evt.button.x), float(evt.button.y));
		mousePoint = _view->mapToScene(mousePoint);

		if (_snapOnGrid)
			mousePoint = Point(int(mousePoint.x), int(mousePoint.y));

		if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_SPACE && !evt.key.repeat && !_editCurrentObject)
			_editCurrentCategory = (_editCurrentCategory++) % MAX_CATEGORIES;
		else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_S && !evt.key.repeat)
			toggleSnapGrid();
		
		if (evt.type == SDL_MOUSEMOTION)
		{
			if (_editCurrentCell)
			{
				killObject(_editCurrentCell);
				_editCurrentCell = nullptr;
			}

			if(!_editCurrentObject)
				_editCurrentCell = new EditableObject(this, RectF(mousePoint.x, mousePoint.y, 1, 1), _editCurrentCategory, 100);
		
			if (_editCurrentObject)
				_editCurrentObject->setSize(mousePoint - _editCurrentObject->rect().pos + PointF(1, 1));
		}
		else if (evt.button.type == SDL_MOUSEBUTTONDOWN && evt.button.button == SDL_BUTTON_LEFT)
		{
			if (_editCurrentObject)
				_editCurrentObject = nullptr;
			else
			{
				_editCurrentObject = new EditableObject(this, RectF(mousePoint.x, mousePoint.y, 1, 1), _editCurrentCategory, 101);
				_editObjects.push_back(_editCurrentObject);
			}
		}
		//else if (evt.button.button == SDL_BUTTON_RIGHT)
			//new Slime(this, { mousePoint.x, mousePoint.y });
	}
}

void GameScene::toggleEditMode()
{ 
	if (_editMode)
		saveEditor();
	else
		loadEditor();

	_editMode = !_editMode; 

	if (_editCurrentCell)
	{
		killObject(_editCurrentCell);
		_editCurrentCell = nullptr;
	}
}

void GameScene::loadEditor()
{
	// freeze game objects
	auto& allObjects = objects();
	for (auto& obj : allObjects)
		obj->setFreezed(true);

	// generate grid
	for (float x = _rect.left(); x < _rect.right(); x++)
		for (float y = _rect.top(); y < _rect.bottom(); y++)
		{
			_editGrid.push_back(new RenderableObject(this, RectF(x, y, 1, 1, _rect.yUp), nullptr, 99));
			_editGrid.back()->setBorderColor(Color(100, 100, 100));
		}

	// load objects from json file
	std::ifstream f("leveleditor.json");
	if (!f.is_open())
		return;
	nlohmann::json data = nlohmann::json::parse(f);
	std::vector<std::string> deserializedObjects = data.get<std::vector<std::string>>();
	for (auto& obj : deserializedObjects)
		_editObjects.push_back(new EditableObject(this, obj));
	f.close();

	// toggle manual camera
	toggleCameraManual();
}

void GameScene::saveEditor()
{
	// save objects to json file
	std::ofstream f("leveleditor.json");
	if (!f.is_open())
		return;
	std::vector <std::string> serializedObjects;
	for (auto& obj : _editObjects)
	{
		serializedObjects.push_back(obj->serialize());
		killObject(obj);
	}
	nlohmann::json j = serializedObjects;
	f << j.dump();
	f.close();

	// remove editor objects
	_editObjects.clear();
	for (auto& obj : _editGrid)
		killObject(obj);
	_editGrid.clear();

	// unfreeze game objects
	auto& allObjects = objects();
	for (auto& obj : allObjects)
		obj->setFreezed(false);

	// toggle manual camera
	toggleCameraManual();
}
