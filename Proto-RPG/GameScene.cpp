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

GameScene::GameScene(const RectF& r, float dt)
	: Scene(r)
{
	_dt = dt;
	_timeToSimulate = 0;
	_player = nullptr;
	_cameraZoomVel = 0.1f;
	_left_pressed = false;
	_right_pressed = false;
	_jump_pressed = false;
	_run_pressed = false;
	_collidersVisible = false;

	// setup view (specific for super mario bros)
	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, -12, 16, 15));
}

void GameScene::update(float timeToSimulate)
{
	Scene::update(timeToSimulate);

	if (!_active)
		return;

	// controls
	if (_right_pressed && !_left_pressed)
		_player->move(Direction::RIGHT);
	else if (_left_pressed && !_right_pressed)
		_player->move(Direction::LEFT);
	else if(_left_pressed && _right_pressed)
		_player->move(Direction::NONE);
	else
		_player->move(Direction::NONE);
	_player->jump(_jump_pressed);
	_player->run(_run_pressed);

	// physics
	_timeToSimulate += timeToSimulate;
	while (_timeToSimulate >= _dt)
	{
		for (auto& layer : _sortedObjects)
			for (auto& obj : layer.second)
				if(!obj->freezed())
					obj->update(_dt);

		_timeToSimulate -= _dt;
	}

	// camera scroll
	// center view on player with left margin (7 scene units) w/o going back
	//_view->setX(std::max(_view->rect().pos.x, _mario->rect().pos.x - 7));
	// center view on player with left margin (7 scene units) with going back
	_view->setX(_player->rect().pos.x - 7);
	_view->setY(_player->rect().pos.y - 7);
}

void GameScene::event(SDL_Event& evt)
{
	Scene::event(evt);

	if (evt.type == SDL_KEYDOWN && (evt.key.keysym.scancode == SDL_SCANCODE_RETURN || evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE))
		Game::instance()->pushScene(Menu::pauseMenu());
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_H)
		_player->die();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_C && !evt.key.repeat)
		toggleColliders();
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
	_right_pressed = state[SDL_SCANCODE_RIGHT];
	_left_pressed = state[SDL_SCANCODE_LEFT];
	_jump_pressed = state[SDL_SCANCODE_SPACE];
	_run_pressed = state[SDL_SCANCODE_Z];
}