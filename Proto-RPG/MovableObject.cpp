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
	_yDir = Direction::NONE;
	_vel = _velPrev = { 0, 0 };

	defaultPhysics();
}

void MovableObject::defaultPhysics()
{
	_velMin = { 0.01f, 0.01f };
	_velMax = { 4, 4 };

	_moveForce = { 100, 100 };
	_frictionForce = { 100, 100 };
}

void MovableObject::velClip(float vx, float vy)
{
	_vel.x = std::max(_vel.x, -vx);
	_vel.x = std::min(_vel.x, vx);
	_vel.y = std::max(_vel.y, -vy);
	_vel.y = std::min(_vel.y, vy);
}

void MovableObject::velAdd(const Vec2Df& amount)
{
	_vel += amount;

	// max velocity clipping
	velClip(_velMax.x, _velMax.y);

	// min velocity clipping
	if (_xDir == Direction::NONE && std::abs(_vel.x) < _velMin.x)
		_vel.x = 0;
	if (_yDir == Direction::NONE && std::abs(_vel.y) < _velMin.y)
		_vel.y = 0;
}

void MovableObject::move(Direction xDir, Direction yDir)
{
	_xDir = xDir;
	_yDir = yDir;
}

void MovableObject::update(float dt)
{
	RenderableObject::update(dt);

	// velocity backup (useful to determine object state)
	_velPrev = _vel;

	// apply forces
	if (_xDir != Direction::NONE)
		velAdd(dir2vec(_xDir) * _moveForce.x);
	if (_yDir != Direction::NONE)
		velAdd(dir2vec(_yDir) * _moveForce.y);
	if (_xDir == Direction::NONE)
		velAdd({ -_vel.versX() * _frictionForce.x * dt, 0 });
	if (_yDir == Direction::NONE)
		velAdd({ 0, -_vel.versY() * _frictionForce.y * dt });

	// move
	_rect.pos += _vel * dt;
}