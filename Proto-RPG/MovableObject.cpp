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
	_x_dir = Direction::NONE;
	_vel = { 0, 0 };

	defaultPhysics();
}

void MovableObject::defaultPhysics()
{
	_y_gravity = 0;
	_y_vel_jmp = 15;
	_y_vel_max = 15;
	_y_vel_min = 0.01f;

	_x_vel_max = 6;	// 10 in running mode
	_x_vel_min = 0.3f;
	_x_acc = 8;		// 13 in running mode
	_x_dec_rel = 11;
	_x_dec_skd = 23;
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
	velClip(_x_vel_max, _y_vel_max);

	// min velocity clipping
	if (_x_dir == Direction::NONE && std::abs(_vel.x) < _x_vel_min)
		_vel.x = 0; // checking _x_dir to allow skidding
	if (std::abs(_vel.y) < _y_vel_min)
		_vel.y = 0;
}

void MovableObject::move(Direction dir)
{
	_x_dir = dir;
}

void MovableObject::jump()
{
	if (!midair())
		velAdd(Vec2Df(0, -_y_vel_jmp));
}

bool MovableObject::skidding() const
{
	return (_x_dir == Direction::RIGHT && _vel.x < 0) ||
		   (_x_dir == Direction::LEFT  && _vel.x > 0);
}

bool MovableObject::grounded() const
{
	return _vel.y == 0 && _prev_vel.y > 0;
}

bool MovableObject::falling() const
{
	return _vel.y > 0 && _prev_vel.y <= 0;
}

bool MovableObject::midair() const
{
	return _vel.y != 0 || (_vel.y == 0 && _prev_vel.y < 0);
}

void MovableObject::update(float dt)
{
	RenderableObject::update(dt);

	// velocity backup (useful to determine object state)
	_prev_vel = _vel;

	// apply gravity acceleration
	velAdd(Vec2Df(0, _y_gravity * dt));

	// apply horizontal accelerations and decelerations
	if (_x_dir == Direction::RIGHT && _vel.x >= 0)
		velAdd(Vec2Df(_x_acc * dt, 0));
	else if (_x_dir == Direction::LEFT && _vel.x <= 0)
		velAdd(Vec2Df(-_x_acc * dt, 0));
	else if (_x_dir == Direction::NONE)
		velAdd(Vec2Df(-_vel.versX() * _x_dec_rel * dt, 0)); // mimics friction
	else if (skidding())
		velAdd(Vec2Df(-_vel.versX() * _x_dec_skd * dt, 0));

	// move
	_rect.pos += _vel * dt;
}