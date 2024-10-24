// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Link.h"
#include "SpriteFactory.h"
#include "Audio.h"
#include "AnimatedSprite.h"
#include "Game.h"

using namespace agp;

Link::Link(Scene* scene, const PointF& pos)
	: DynamicObject(scene, RectF( pos.x + 1 / 16.0f, pos.y, 1, 1 ), nullptr)
{
	_collider.adjust(0.2f, 0, -0.2f, -1/16.0f);

	_walking = false;
	_jumping = false;
	_running = false;
	_dying = false;
	_dead = false;
	_invincible = true;

	_x_vel_last_nonzero = 0;

	_sprites["stand"] = SpriteFactory::instance()->get("mario_stand");
	_sprites["walk"] = SpriteFactory::instance()->get("mario_walk");
	_sprites["run"] = SpriteFactory::instance()->get("mario_run");
	_sprites["skid"] = SpriteFactory::instance()->get("mario_skid");
	_sprites["jump"] = SpriteFactory::instance()->get("mario_jump");
	_sprites["die"] = SpriteFactory::instance()->get("mario_die");
	_sprite = _sprites["stand"];
}

void Link::update(float dt)
{
	// physics
	DynamicObject::update(dt);

	// state logic
	if (_jumping && grounded())
		_jumping = false;
	if (_vel.x != 0 && !_jumping)
		_x_vel_last_nonzero = _vel.x;
	_walking = _vel.x != 0;
	_running = std::abs(_vel.x) > 6;

	// animations
	if(_dying)
		_sprite = _sprites["die"];
	else if (_jumping)
		_sprite = _sprites["jump"];
	else if (skidding())
		_sprite = _sprites["skid"];
	else if (_running)
		_sprite = _sprites["run"];
	else if(_walking)
		_sprite = _sprites["walk"];
	else
		_sprite = _sprites["stand"];

	// x-mirroring
	if ((_vel.x < 0 && !_jumping) || _x_vel_last_nonzero < 0)
		_flip = SDL_FLIP_HORIZONTAL;
	else
		_flip = SDL_FLIP_NONE;
}

void Link::move(Direction dir)
{
	if (_dying || _dead)
		return;

	MovableObject::move(dir);
}

void Link::jump(bool on)
{
	if (_dying || _dead)
		return;

	if (on && !midair())
	{
		velAdd(Vec2Df(0, -_y_vel_jmp));

		if (std::abs(_vel.x) < 9)
			_y_gravity = 25;
		else
			_y_gravity = 21;

		_jumping = true;
		Audio::instance()->playSound("jump-small");
	}
	else if (!on && midair() && !_dying)
		_y_gravity = 100;
}

void Link::run(bool on)
{
	if (midair())
		return;

	if (on)
	{
		_x_vel_max = 10;
		_x_acc = 13;
	}
	else
	{
		_x_vel_max = 6;	
		_x_acc = 8;
	}
}

void Link::die()
{
	if (_dying)
		return;

	_dying = true;
	_collidable = false;
	_y_gravity = 0;
	_vel = { 0,0 };
	_x_dir = Direction::NONE;
	Audio::instance()->haltMusic();
	Audio::instance()->playSound("death");
	Game::instance()->freeze(true);

	schedule("dying", 0.5f, [this]()
		{
			_y_gravity = 25;
			velAdd(Vec2Df(0, -_y_vel_jmp));
			schedule("die", 3, [this]()
				{
					_dead = true;
					Game::instance()->gameover();
				});
		});
}

void Link::hurt()
{
	// TODO: powerdown (e.g. if Mario is big, becomes small)
	//die();
}