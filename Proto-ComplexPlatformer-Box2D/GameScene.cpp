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
#include "SpriteFactory.h"
#include "Game.h"
#include "Menu.h"
#include "RigidObject.h"
#include "Player.h"
#include "Box.h"

using namespace agp;

GameScene::GameScene(const RectF& r, float dt)
	: Scene(r)
{
	_dt = dt;
	_timeToSimulate = 0;
	_cameraTranslateVel = { 4.0f, 0 };
	_cameraZoomVel = 0.1f;
	_player = nullptr;
	_left_pressed = false;
	_right_pressed = false;
	_jump_pressed = false;
	_run_pressed = false;
	_collidersVisible = false;

	// box2D world init
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = { 0.0f, -24.0f };
	worldDef.enableSleep = true;
	_worldId = b2CreateWorld(&worldDef);
	if (!b2World_IsValid(_worldId))
		throw "Cannot initialize Box2D world";

	// setup view
	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, 0, 24, 13, true));
}

GameScene::~GameScene()
{
	if (b2World_IsValid(_worldId))
		b2DestroyWorld(_worldId);
}

void GameScene::update(float timeToSimulate)
{
	Scene::update(timeToSimulate);

	if (!_active)
		return;

	// controls
	if (_right_pressed && !_left_pressed)
	{
		//_view->move({ _cameraTranslateVel.x * timeToSimulate, 0 });
		_player->move(Direction::RIGHT);
	}
	else if (_left_pressed && !_right_pressed)
	{
		//_view->move({ -_cameraTranslateVel.x * timeToSimulate, 0 });
		_player->move(Direction::LEFT);
	}
	else if (!_left_pressed && !_right_pressed)
		_player->move(Direction::NONE);

	if (_jump_pressed)
		_player->jump();

	_player->setRunning(_run_pressed);

	// physics
	/*_timeToSimulate += timeToSimulate;
	while (_timeToSimulate >= _dt)
	{
		b2World_Step(_worldId, _dt, _subStepCount);
		_timeToSimulate -= _dt;
	}*/
	b2World_Step(_worldId, timeToSimulate, _subStepCount);

	// collisions (logic)
	b2ContactEvents contactEvents = b2World_GetContactEvents(_worldId);
	for (int i = 0; i < contactEvents.beginCount; ++i)
	{
		b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;
	
		RigidObject* objA = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(beginEvent->shapeIdA)));
		RigidObject* objB = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(beginEvent->shapeIdB)));

		Vec2Df normal = { beginEvent->manifold.normal.x, beginEvent->manifold.normal.y };

		objA->collision(objB, true, normal, beginEvent->shapeIdA, beginEvent->shapeIdB);
		objB->collision(objA, true, -normal, beginEvent->shapeIdB, beginEvent->shapeIdA);
	}
	for (int i = 0; i < contactEvents.endCount; ++i)
	{
		b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
		RigidObject* objA = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(endEvent->shapeIdA)));
		RigidObject* objB = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(endEvent->shapeIdB)));

		// once a contact has ended, contact normal is undefined
		objA->collision(objB, false, Vec2Df(), endEvent->shapeIdA, endEvent->shapeIdB);
		objB->collision(objA, false, Vec2Df(), endEvent->shapeIdB, endEvent->shapeIdA);
	}

	// triggers (=sensors in Box2D)
	b2SensorEvents sensorEvents = b2World_GetSensorEvents(_worldId);
	for (int i = 0; i < sensorEvents.beginCount; ++i)
	{
		b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
		RigidObject* objA = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(beginTouch->sensorShapeId)));
		RigidObject* objB = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(beginTouch->visitorShapeId)));
		objA->collision(objB, true, Vec2Df(), beginTouch->sensorShapeId, beginTouch->visitorShapeId);
	}

	// logic and animations
	auto allObjects = objects();
	for (auto& obj : allObjects)
		if (!obj->freezed())
			obj->update(timeToSimulate);

	// camera
	// center view on player with left margin (12 scene units) with going back
	//_view->setX(_player->rect().pos.x - 12);
	_view->setX(std::max(_view->rect().pos.x, _player->rect().pos.x - 11));
}

void GameScene::event(SDL_Event& evt)
{
	Scene::event(evt);

	if (evt.type == SDL_KEYDOWN)
	{
		if(evt.key.keysym.scancode == SDL_SCANCODE_RETURN || evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			Game::instance()->pushScene(Menu::pauseMenu());
		else if (evt.key.keysym.scancode == SDL_SCANCODE_SPACE && !evt.key.repeat)
			player()->jump();
		else if (evt.key.keysym.scancode == SDL_SCANCODE_D && !evt.key.repeat)
			player()->dash();
		else if (evt.key.keysym.scancode == SDL_SCANCODE_C && !evt.key.repeat)
			toggleColliders();
	}
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

		if (evt.button.button == SDL_BUTTON_LEFT)
			new Box(this, RotatedRectF({ mousePoint.x, mousePoint.y }, { 1,1 }, 0, true));
	}

	// update control flags
	const Uint8* state = SDL_GetKeyboardState(0);
	_right_pressed = state[SDL_SCANCODE_RIGHT];
	_left_pressed = state[SDL_SCANCODE_LEFT];
	_run_pressed = state[SDL_SCANCODE_Z];
}