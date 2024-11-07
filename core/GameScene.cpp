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
	else
	{
		_view->setX(_player->rect().pos.x - _view->rect().size.x / 2);
		_view->setY(_player->rect().pos.y - _view->rect().size.y / 2);
	}
}

void GameScene::event(SDL_Event& evt)
{
	Scene::event(evt);

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_C && !evt.key.repeat)
		toggleColliders();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_R && !evt.key.repeat)
		toggleRects();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_M && !evt.key.repeat)
		toggleCamera();
	else if (evt.type == SDL_MOUSEWHEEL)
	{
		if (evt.wheel.y > 0)
			_view->scale(1 - _cameraZoomVel);
		else if (evt.wheel.y < 0)
			_view->scale(1 + _cameraZoomVel);
	}
}