// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Player.h"
#include "KinematicObject.h"
#include "Terrain.h"
#include "GameScene.h"
#include "SpriteFactory.h"
#include "AnimatedSprite.h"

using namespace agp;

Player::Player(GameScene* scene, PointF pos)
	: DynamicObject(scene, RotatedRectF( pos.x, pos.y, 2, 3.2f, 0, true ), nullptr)
{
	// capsule-shaped player is generally the best choice
	b2Capsule capsule;
	capsule = ((_obb - _obb.center - Vec2Df(0, 0.5f)) * 0.6f).toCapsule();
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1;
	shapeDef.friction = 0.2f;
	shapeDef.restitution = 0.0;
	shapeDef.enableContactEvents = true;
	b2CreateCapsuleShape(_bodyId, &shapeDef, &capsule);

	// box-shaped player might get stuck on tiled platforms
	/*b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1;
	shapeDef.friction = 0.2f;
	shapeDef.restitution = 0.0;
	shapeDef.enableContactEvents = true;
	b2Polygon boxDef = b2MakeOffsetBox(_obb.size.x / 2, _obb.size.y / 2, { 0, 0 }, { 1, 0 });
	b2CreatePolygonShape(_bodyId, &shapeDef, &boxDef);*/

	_xVelMax = 5;
	_yVelMax = 10;
	_xDashForce = 3000;
	_running = false;
	_dashing = false;

	_sprites["idle"] = SpriteFactory::instance()->get("player_idle");
	_sprites["jump"] = SpriteFactory::instance()->get("player_jump");
	_sprites["walk"] = SpriteFactory::instance()->get("player_walk");
	_sprites["dash"] = SpriteFactory::instance()->get("player_dash");
	_sprite = _sprites["idle"];
	_lastNonZeroXDir = _xDir;

	b2Body_SetFixedRotation(_bodyId, true);
}

void Player::setRunning(bool running)
{
	_running = running;
	_xVelMax = _running ? 10.0f : 5.0f;
}

void Player::move(Direction dir)
{
	if (dir != Direction::NONE && dir != _lastNonZeroXDir)
		_lastNonZeroXDir = dir;

	DynamicObject::move(dir);
}

void Player::dash()
{
	if (_dashing)
		return;

	_dashing = true;
	_xVelMax = 1000;

	schedule("dashing_off", 0.5f, [this]()
		{
			_dashing = false;
			_xVelMax = 5;
		});
}

void Player::update(float dt)
{
	DynamicObject::update(dt);

	// dashing = applying continous (big) force while dashing flag is on
	if(_dashing)
		b2Body_ApplyForceToCenter(_bodyId, (dir2vec(_lastNonZeroXDir) * Vec2Df(_xDashForce, 0)).toB2(), true);

	// raycast ground detection when onGround to solve multiple's ground ambiguity
	// that occurs when transitioning between two adjacent terrains
	Terrain* terrain = nullptr;
	if (onGround())
	{
		b2RayResult raycast = b2World_CastRayClosest(
			dynamic_cast<GameScene*>(_scene)->worldId(),
			{ _obb.center.x, _obb.center.y },
			{ 0, -_obb.size.y / 2 - 1 }, b2DefaultQueryFilter());

		if (raycast.hit)
		{
			RigidObject* obj = ((RigidObject*)b2Body_GetUserData(b2Shape_GetBody(raycast.shapeId)));
			terrain = dynamic_cast<Terrain*>(obj);
		}

		// give player a boost to compensate gravity along slope
		if (terrain && terrain->slope())
		{
			Vec2Df tangent = terrain->normal().perp().norm();
			b2Vec2 b2gravity = b2World_GetGravity(dynamic_cast<GameScene*>(_scene)->worldId());
			float gravity = Vec2Df(b2gravity.x, b2gravity.y).mag();
			float mass = b2Body_GetMass(_bodyId);
			float gravityForce = mass * gravity;
			float gravityAlongSlope = gravityForce * (Vec2Df(0, -1).dot(tangent));
			Vec2Df _compensationForce = -gravityAlongSlope * tangent;
			b2Body_ApplyForceToCenter(_bodyId, { _compensationForce.x, _compensationForce.y }, true);
		}
	}

	// animations switch
	if (_dashing)
		_sprite = _sprites["dash"];
	else if (!onGround())
		_sprite = _sprites["jump"];
	else if (_running || (std::fabsf(vel().x) > 0.5f) && dynamic_cast<StaticObject*>(terrain))
	{
		_sprite = _sprites["walk"];
		dynamic_cast<AnimatedSprite*>(_sprite)->setFPS(std::abs(vel().x) * 5);
	}
	else
		_sprite = _sprites["idle"];

	// animation flip
	if (_lastNonZeroXDir == Direction::LEFT)
		_flip = SDL_FLIP_HORIZONTAL;
	else
		_flip = SDL_FLIP_NONE;
}