// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "MovableObject.h"

using namespace agp;

MovableObject::MovableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	RenderableObject(scene, rect, sprite, layer)
{
	// default movement (stand)
	_xDir = Direction::NONE;
	_vel = { 0, 0 };

	defaultPhysics();
}

void MovableObject::defaultPhysics()
{
	_yGravityForce = 100;
	_yJumpImpulse = 15;
	_yVelMax = 15;
	_yVelMin = 0.01f;
	_xVelMax = 6;			// 10 in running mode
	_xVelMin = 0.3f;
	_xMoveForce = 8;		// 13 in running mode
	_xFrictionForce = 11;
	_xSkiddingForce = 23;
}

void MovableObject::velClip(float vx, float vy)
{
	_vel.x = std::max(_vel.x, -vx);
	_vel.x = std::min(_vel.x, vx);
	_vel.y = std::max(_vel.y, -vy);
	_vel.y = std::min(_vel.y, vy);
}

void MovableObject::velAdd(Vec2Df amount)
{
	_vel += amount;

	// max velocity clipping
	velClip(_xVelMax, _yVelMax);

	// min velocity clipping
	if (_xDir == Direction::NONE && std::abs(_vel.x) < _xVelMin)
		_vel.x = 0; // checking _xDir to allow skidding
	if (std::abs(_vel.y) < _yVelMin)
		_vel.y = 0;
}

void MovableObject::move(Direction dir)
{
	_xDir = dir;
}

void MovableObject::jump()
{
	if (!midair())
		velAdd(Vec2Df(0, -_yJumpImpulse));
}

bool MovableObject::skidding() const
{
	return (_xDir == Direction::RIGHT && _vel.x < 0) ||
		   (_xDir == Direction::LEFT  && _vel.x > 0);
}

bool MovableObject::grounded() const
{
	return _vel.y == 0 && _prevVel.y > 0;
}

bool MovableObject::falling() const
{
	return _vel.y > 0 && _prevVel.y <= 0;
}

bool MovableObject::midair() const
{
	return _vel.y != 0 || (_vel.y == 0 && _prevVel.y < 0);
}

void MovableObject::update(float dt)
{
	RenderableObject::update(dt);

	// velocity backup (useful to determine object state)
	_prevVel = _vel;

	// Semi-implicit Euler integration
	// 1) update velocity (=apply accelerations = forces)
	// 2) use updated velocity to update position

	// apply gravity acceleration
	velAdd(Vec2Df(0, _yGravityForce * dt));

	// apply horizontal accelerations and decelerations
	if (_xDir == Direction::RIGHT && _vel.x >= 0)
		velAdd(Vec2Df(_xMoveForce * dt, 0));
	else if (_xDir == Direction::LEFT && _vel.x <= 0)
		velAdd(Vec2Df(-_xMoveForce * dt, 0));
	else if (_xDir == Direction::NONE)
		velAdd(Vec2Df(-_vel.versX() * _xFrictionForce * dt, 0)); // mimics friction
	else if (skidding())
		velAdd(Vec2Df(-_vel.versX() * _xSkiddingForce * dt, 0));

	// move
	setPos(pos() + _vel * dt);
}