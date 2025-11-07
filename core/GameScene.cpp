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
#include "View.h"
#include "Game.h"
#include "Audio.h"
#include "OverlayScene.h"
#include "EditorScene.h"
#include "EditorUI.h"
#include "timeUtils.h"

using namespace agp;

GameScene::GameScene(const RectF& rect, const Point& pixelUnitSize, float dt)
	: Scene(rect, pixelUnitSize), _quadtree(rect)
{
	_dt = dt;
	_timeToSimulateAccum = 0;
	_player = nullptr;
	_cameraZoomVel = 0.1f;
	_cameraTranslateVel = { 500, 500 };
	_collidersVisible = false;
	_cameraManual = false;
	_cameraFollowsPlayer = true;
	_displayGameSceneOnly = false;
	_autoKillWhenOutsideScene = true;
	_useQuadtree = false;
	_jsonPath = std::string(SDL_GetBasePath()) + "/EditorScene.json";

	_view = new View(this, _rect);
	float ar = Game::instance()->aspectRatio();
	if(ar)
		_view->setFixedAspectRatio(ar);
}

void GameScene::newObject(Object* obj)
{
	Scene::newObject(obj);

	if(_useQuadtree)
		_quadtree.add(obj);
}

void GameScene::killObject(Object* obj)
{
	Scene::killObject(obj);

	if (_useQuadtree)
		_quadtree.remove(obj);
}

void GameScene::objectMoved(Object* obj)
{
	Scene::objectMoved(obj);

	if (obj->killed())
		return;

	if (!_rect.contains(obj->rect())) 
	{
		if(_autoKillWhenOutsideScene && obj != _player)
			killObject(obj);   
		return;
	}

	static Profiler quadtreeUpdateProfiler("quadtree update", 5000);
	quadtreeUpdateProfiler.begin();
	if (_useQuadtree)
		_quadtree.update(obj);
	quadtreeUpdateProfiler.end();
}

Objects GameScene::objects()
{
	return Scene::objects();
}

Objects GameScene::objects(const RectF& cullingRect)
{
	if (_useQuadtree)
		return _quadtree.queryObjects(cullingRect);
	else
		return Scene::objects(cullingRect);
}

Objects GameScene::objects(const PointF& containPoint)
{
	std::vector<Object*> candidates = _quadtree.queryObjects(RotatedRectF(containPoint, { 1,1 }, 0, _rect.yUp).toRect());
	std::vector<Object*> results;
	for (auto obj : candidates)
		if (obj->contains(containPoint))
			results.push_back(obj);

	return results;
}

bool GameScene::isEmpty(const RectF& rect)
{
	if (_useQuadtree)
		return _quadtree.queryObjects(rect).empty();
	else
		return Scene::isEmpty(rect);
}

void GameScene::render()
{
	if (_active)
	{
		if (!_displayGameSceneOnly)
			for (auto& bgScene : _backgroundScenes)
				bgScene->render();

		_view->render();

		if(!_displayGameSceneOnly)
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
	static Profiler updateWorldProfiler("updateWorld", 5000);
	updateWorldProfiler.begin();

	// semi-fixed timestep
	_timeToSimulateAccum += timeToSimulate;
	while (_timeToSimulateAccum >= _dt)
	{
		for (auto& obj : _objects)
			if (!obj->freezed())
				obj->update(_dt);		// physics, collision, logic, animation

		_timeToSimulateAccum -= _dt;
	}


	updateWorldProfiler.end();
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
	else if (evt.type == SDL_MOUSEWHEEL && _cameraManual)
	{
		if (evt.wheel.y > 0)
			_view->scale(1 - _cameraZoomVel);
		else if (evt.wheel.y < 0)
			_view->scale(1 + _cameraZoomVel);
	}

	// open editor
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_E && !evt.key.repeat)
	{
		EditorUI* editorUI = new EditorUI();
		EditorScene* editorScene = new EditorScene(this, editorUI, _jsonPath);
		Game::instance()->pushScene(editorScene);
		Game::instance()->pushScene(editorUI);
	}
}
