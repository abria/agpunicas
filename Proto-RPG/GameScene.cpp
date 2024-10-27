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
#include "CollidableObject.h"
#include "View.h"
#include "SpriteFactory.h"
#include "Game.h"
#include "Menu.h"
#include "Audio.h"
#include "Link.h"

using namespace agp;

GameScene::GameScene(const RectF& rect, const Point& pixelUnitSize, float dt)
	: Scene(rect, pixelUnitSize)
{
	_dt = dt;
	_timeToSimulate = 0;
	_player = nullptr;
	_cameraZoomVel = 0.1f;
	_leftPressed = false;
	_rightPressed = false;
	_upPressed = false;
	_downPressed = false;
	_collidersVisible = false;

	// setup view (SNES aspect ratio)
	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, 0, 8, 7.5));
}

void GameScene::update(float timeToSimulate)
{
	Scene::update(timeToSimulate);

	if (!_active)
		return;

	// controls
	Direction xDir = Direction::NONE;
	Direction yDir = Direction::NONE;
	if (_rightPressed && !_leftPressed)
		xDir = Direction::RIGHT;
	if (_leftPressed && !_rightPressed)
		xDir = Direction::LEFT;
	if (_upPressed && !_downPressed)
		yDir = Direction::UP;
	if (_downPressed && !_upPressed)
		yDir = Direction::DOWN;
	_player->move(xDir, yDir);

	// semi-fixed timestep
	_timeToSimulate += timeToSimulate;
	while (_timeToSimulate >= _dt)
	{
		for (auto& layer : _sortedObjects)
			for (auto& obj : layer.second)
				if(!obj->freezed())
					obj->update(_dt);		// physics, collision, logic, animation

		_timeToSimulate -= _dt;
	}

	// camera: center view on player 
	_view->setX(_player->rect().pos.x - 3.5f);
	_view->setY(_player->rect().pos.y - 3.5f);
}

void GameScene::event(SDL_Event& evt)
{
	Scene::event(evt);

	if (evt.type == SDL_KEYDOWN && (evt.key.keysym.scancode == SDL_SCANCODE_RETURN || evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
		Game::instance()->pushScene(Menu::pauseMenu());
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_H)
		_player->die();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_A)
		_player->attack();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_C && !evt.key.repeat)
		toggleColliders();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_R && !evt.key.repeat)
		toggleRects();
	else if (evt.type == SDL_MOUSEWHEEL)
	{
		if (evt.wheel.y > 0)
			_view->scale(1 - _cameraZoomVel);
		else if (evt.wheel.y < 0)
			_view->scale(1 + _cameraZoomVel);
	}
	else if (evt.type == SDL_MOUSEBUTTONDOWN)
	{
		PointF mousePoint(float(evt.button.x), float(evt.button.y));
		mousePoint = _view->mapToScene(mousePoint);

		if (evt.button.button == SDL_BUTTON_RIGHT)
		{
			for (auto& obj : objects(_view->rect()))
				if (obj->contains(mousePoint))
					killObject(obj);
		}
		else if (evt.button.button == SDL_BUTTON_LEFT)
			new RenderableObject(this, RectF(floor(mousePoint.x), floor(mousePoint.y), 1, 1), SpriteFactory::instance()->get("brick"));
	}

	// update control flags
	const Uint8* state = SDL_GetKeyboardState(0);
	_rightPressed = state[SDL_SCANCODE_RIGHT];
	_leftPressed = state[SDL_SCANCODE_LEFT];
	_upPressed = state[SDL_SCANCODE_UP];
	_downPressed = state[SDL_SCANCODE_DOWN];
}