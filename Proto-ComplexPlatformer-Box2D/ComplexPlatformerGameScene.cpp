// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ComplexPlatformerGameScene.h"
#include "RenderableObject.h"
#include "View.h"
#include "SpriteFactory.h"
#include "Game.h"
#include "Menu.h"
#include "RigidObject.h"
#include "Player.h"
#include "Box.h"
#include "Slime.h"
#include "OverlayScene.h"

using namespace agp;

ComplexPlatformerGameScene::ComplexPlatformerGameScene(const RectF& rect, const Point& pixelUnitSize, float dt)
	: GameScene(rect, pixelUnitSize, dt)
{
	// box2D world init
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = { 0.0f, -24.0f };
	worldDef.enableSleep = true;
	_worldId = b2CreateWorld(&worldDef);
	if (!b2World_IsValid(_worldId))
		throw "Cannot initialize Box2D world";

	// setup view
	_view->setRect(RectF(0, 0, 24, 13, true));
}

ComplexPlatformerGameScene::~ComplexPlatformerGameScene()
{
	if (b2World_IsValid(_worldId))
		b2DestroyWorld(_worldId);
}

void ComplexPlatformerGameScene::updateControls(float timeToSimulate)
{
	if (_cameraManual)
		return;

	Player* player = dynamic_cast<Player*>(_player);
	const Uint8* keyboard = SDL_GetKeyboardState(0);

	if (keyboard[SDL_SCANCODE_RIGHT] && !keyboard[SDL_SCANCODE_LEFT])
		player->move(Direction::RIGHT);
	else if (keyboard[SDL_SCANCODE_LEFT] && !keyboard[SDL_SCANCODE_RIGHT])
		player->move(Direction::LEFT);
	else if (keyboard[SDL_SCANCODE_LEFT] && keyboard[SDL_SCANCODE_RIGHT])
		player->move(Direction::NONE);
	else
		player->move(Direction::NONE);

	player->setRunning(keyboard[SDL_SCANCODE_Z]);
}

void ComplexPlatformerGameScene::updateCamera(float timeToSimulate)
{
	if (_cameraManual)
	{
		GameScene::updateCamera(timeToSimulate);
		return;
	}

	_view->setX(std::max(_view->rect().pos.x, _player->rect().pos.x - 11));
}

void ComplexPlatformerGameScene::updateWorld(float timeToSimulate)
{
	// semi-fixed time step
	_timeToSimulateAccum += timeToSimulate;
	while (_timeToSimulateAccum >= _dt)
	{
		// Box2D physics
		b2World_Step(_worldId, _dt, _subStepCount);
		_timeToSimulateAccum -= _dt;

		// collisions (logic)
		// 'standard' collisions
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

		// triggers (=sensors in Box2D) collisions
		b2SensorEvents sensorEvents = b2World_GetSensorEvents(_worldId);
		for (int i = 0; i < sensorEvents.beginCount; ++i)
		{
			b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
			RigidObject* objA = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(beginTouch->sensorShapeId)));
			RigidObject* objB = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(beginTouch->visitorShapeId)));
			objA->collision(objB, true, Vec2Df(), beginTouch->sensorShapeId, beginTouch->visitorShapeId);
		}
		for (int i = 0; i < sensorEvents.endCount; ++i)
		{
			b2SensorEndTouchEvent* endTouch = sensorEvents.endEvents + i;
			RigidObject* objA = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(endTouch->sensorShapeId)));
			RigidObject* objB = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(endTouch->visitorShapeId)));
			objA->collision(objB, false, Vec2Df(), endTouch->sensorShapeId, endTouch->visitorShapeId);
		}

		// hit collisions
		// @TODO

		// logic and animations
		auto allObjects = objects();
		for (auto& obj : allObjects)
			if (!obj->freezed())
				obj->update(timeToSimulate);
	}
}

void ComplexPlatformerGameScene::event(SDL_Event& evt)
{
	GameScene::event(evt);

	Player* player = dynamic_cast<Player*>(_player);

	if (evt.type == SDL_KEYDOWN)
	{
		if(evt.key.keysym.scancode == SDL_SCANCODE_RETURN || evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			Game::instance()->pushScene(Menu::pauseMenu());
		else if (evt.key.keysym.scancode == SDL_SCANCODE_SPACE && !evt.key.repeat)
			player->jump();
		else if (evt.key.keysym.scancode == SDL_SCANCODE_D && !evt.key.repeat)
			player->dash();
		else if (evt.key.keysym.scancode == SDL_SCANCODE_F && !evt.key.repeat)
			player->fire();
	}
	else if (evt.type == SDL_MOUSEBUTTONDOWN)
	{
		PointF mousePoint(float(evt.button.x), float(evt.button.y));
		mousePoint = _view->mapToScene(mousePoint);

		if (evt.button.button == SDL_BUTTON_LEFT)
			new Box(this, RotatedRectF({ mousePoint.x, mousePoint.y }, { 1,1 }, 0, true));
		else if (evt.button.button == SDL_BUTTON_RIGHT)
			new Slime(this, { mousePoint.x, mousePoint.y });
	}
}