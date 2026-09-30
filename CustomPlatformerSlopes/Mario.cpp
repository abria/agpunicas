// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Mario.h"
#include "SpriteFactory.h"
#include "Audio.h"
#include "AnimatedSprite.h"
#include "PlatformerGame.h"
#include "Scene.h"
#include "HUD.h"

using namespace agp;

Mario::Mario(Scene* scene, const PointF& pos)
	: DynamicObject(scene, RectF( pos.x + 1 / 16.0f, pos.y, 1, 1 ), nullptr, 3)
{
	_collider.adjust(0.2f, 0, -0.2f, -1/16.0f);

	_walking = false;
	_jumping = false;
	_canJump = true;
	_running = false;
	_skidding = false;
	_dying = false;
	_dead = false;
	_invincible = false;
	_transformSmall2Super = false;
	_transformSuper2Small = false;
	_super = false;
	_crouch = false;
	_pSpeed = false;
	_facingDir = Direction::RIGHT;

	_fit = false;	// sprites will NOT be expanded to fit the draw rect

	_xLastNonZeroVel = 0;

	_sprites[0]["stand"] = SpriteFactory::instance()->get("mario_stand");
	_sprites[0]["walk"] = SpriteFactory::instance()->get("mario_walk");
	_sprites[0]["run"] = SpriteFactory::instance()->get("mario_run");
	_sprites[0]["pspeed"] = SpriteFactory::instance()->get("mario_pspeed");
	_sprites[0]["skid"] = SpriteFactory::instance()->get("mario_skid");
	_sprites[0]["jump"] = SpriteFactory::instance()->get("mario_jump");
	_sprites[0]["jumpP"] = SpriteFactory::instance()->get("mario_jumpP");
	_sprites[0]["fall"] = SpriteFactory::instance()->get("mario_fall");
	_sprites[0]["die"] = SpriteFactory::instance()->get("mario_die");
	_sprites[0]["small2big"] = SpriteFactory::instance()->get("mario_small2big");

	_sprites[1]["stand"] = SpriteFactory::instance()->get("supermario_stand");
	_sprites[1]["walk"] = SpriteFactory::instance()->get("supermario_walk");
	_sprites[1]["crouch"] = SpriteFactory::instance()->get("supermario_crouch");
	_sprites[1]["run"] = SpriteFactory::instance()->get("supermario_run");
	_sprites[1]["pspeed"] = SpriteFactory::instance()->get("supermario_pspeed");
	_sprites[1]["skid"] = SpriteFactory::instance()->get("supermario_skid");
	_sprites[1]["jump"] = SpriteFactory::instance()->get("supermario_jump");
	_sprites[1]["jumpP"] = SpriteFactory::instance()->get("supermario_jumpP");
	_sprites[1]["fall"] = SpriteFactory::instance()->get("supermario_fall");
	_sprites[1]["die"] = _sprites[0]["die"];
	_sprites[1]["big2small"] = SpriteFactory::instance()->get("supermario_big2small");

	_sprite = _sprites[0]["stand"];
}

void Mario::update(float dt)
{
	// physics
	const float previousX = pos().x;
	DynamicObject::update(dt);

	// state logic
	if (_jumping && grounded())
		_jumping = false;
	if (_vel.x != 0 && !_jumping)
		_xLastNonZeroVel = _vel.x;
	// Passive sliding moves Mario while leaving his stored horizontal velocity at zero.
	_walking = _vel.x != 0 || (_slopeSliding && std::abs(pos().x - previousX) > 0.00001f);
	_running = std::abs(_vel.x) > 6;
	if (_transformSmall2Super && dynamic_cast<AnimatedSprite*>(_sprites[0]["small2big"])->ended())
		powerup(false);
	if (_transformSuper2Small && dynamic_cast<AnimatedSprite*>(_sprites[1]["big2small"])->ended())
		powerdown(false);

	bool _pSpeedHUD = dynamic_cast<PlatformerGame*>(Game::instance())->hud()->pSpeed();
	if(!_pSpeed && _pSpeedHUD)
		Audio::instance()->playSound("PMeter", -1);
	else if ((_pSpeed && !_pSpeedHUD) || _dying)
		Audio::instance()->stopSound("PMeter");
	_pSpeed = _pSpeedHUD;

	_skidding = skidding();
	if (_skidding && !midair())
		Audio::instance()->playSound("Skid", 0, true);

	// animations
	if(_transformSmall2Super)
		_sprite = _sprites[0]["small2big"];
	else if(_transformSuper2Small)
		_sprite = _sprites[1]["big2small"];
	else if(_dying)
		_sprite = _sprites[_super]["die"];
	else if (_crouch)
		_sprite = _sprites[1]["crouch"];
	else if (_jumping && _pSpeed)
		_sprite = _sprites[_super]["jumpP"];
	else if (falling())
		_sprite = _sprites[_super]["fall"];
	else if (_jumping)
		_sprite = _sprites[_super]["jump"];
	else if (_skidding)
		_sprite = _sprites[_super]["skid"];
	else if (_pSpeed && _running)
		_sprite = _sprites[_super]["pspeed"];
	else if (_running)
		_sprite = _sprites[_super]["run"];
	else if(_walking)
		_sprite = _sprites[_super]["walk"];
	else
		_sprite = _sprites[_super]["stand"];

	// x-mirroring
	// Mario's sprites face left, hence flip if he faces right)
	if (_facingDir == Direction::RIGHT)
		_flip = SDL_FLIP_HORIZONTAL;
	else
		_flip = SDL_FLIP_NONE;
}

void Mario::move(Direction dir)
{
	if (_dying || _dead)
		return;

	DynamicObject::move(dir);
}

void Mario::jump(bool on)
{
	if (_dying || _dead)
		return;

	if (!_jumping && !on)
		_canJump = true;

	if (on && !midair() && _canJump)
	{
		_canJump = false;
		velAdd(Vec2Df(0, -_yJumpImpulse));

		if (std::abs(_vel.x) < 9)
			_yGravityForce = 25;
		else if (_pSpeed)
			_yGravityForce = 17;
		else
			_yGravityForce = 21;

		_jumping = true;
		Audio::instance()->playSound("Mario Jump");
	}
	else if (!on && midair() && !_dying)
		_yGravityForce = 100;
}

void Mario::bounce()
{
	velAdd(Vec2Df(0, -3*_yJumpImpulse));
}

void Mario::run(bool on)
{
	if (midair() || _dying || _dead)
		return;

	dynamic_cast<PlatformerGame*>(Game::instance())->hud()->setRunning(on && abs(_vel.x) > 6.0f && !skidding());

	if (on)
	{
		_xVelMax = _pSpeed ? 13.0f : 10.0f;
		_xMoveForce = 13.0f;
	}
	else
	{
		_xVelMax = 6.0f;
		_xMoveForce = 8.0f;
	}
}

void Mario::crouch(bool on)
{
	if (midair() || !_super || _crouch == on || _transformSuper2Small)
		return;

	_crouch = on;

	if(on)
		_collider.adjust(0, 0.55f, 0, 0);
	else
		_collider.adjust(0, -0.55f, 0, 0);
}

void Mario::die()
{
	if (_dying)
		return;

	_dying = true;
	_collidable = false;
	_yGravityForce = 0;
	_vel = { 0,0 };
	_xDir = Direction::NONE;
	Audio::instance()->haltMusic();
	Audio::instance()->playSound("Death");
	dynamic_cast<PlatformerGame*>(Game::instance())->freeze(true);

	schedule("dying", 0.5f, [this]()
		{
			_yGravityForce = 25;
			velAdd(Vec2Df(0, -_yJumpImpulse));
			schedule("die", 3, [this]()
				{
					_dead = true;
					dynamic_cast<PlatformerGame*>(Game::instance())->gameover();
				});
		});
}

void Mario::hurt()
{
	if (!_invincible)
		powerdown();
}

void Mario::powerup(bool start)
{
	if (start == true)
	{
		if (_super || _transformSmall2Super)
			return;

		_transformSmall2Super = true;
		_freezedPhysics = true;
		dynamic_cast<PlatformerGame*>(Game::instance())->freeze(true);

		Audio::instance()->playSound("Powerup");

		// warning: this is teleport which may break the CCD engine
		setRect(RectF(pos().x, pos().y - 1, 1, 2));
		_collider.adjust(0, 0.4f, 0, 1);
	}
	else
	{
		dynamic_cast<AnimatedSprite*>(_sprites[0]["small2big"])->reset();
		_transformSmall2Super = false;
		_super = true;
		_freezedPhysics = false;
		dynamic_cast<PlatformerGame*>(Game::instance())->freeze(false);
	}
}

void Mario::powerdown(bool start)
{
	if(start)
	{
		if (!_super)
		{
			die();
			return;
		}

		if (_transformSuper2Small)
			return;

		crouch(false);

		_transformSuper2Small = true;
		_freezedPhysics = true;
		dynamic_cast<PlatformerGame*>(Game::instance())->freeze(true);

		Audio::instance()->playSound("Pipe");

		setFlashingFrequency(10);
		schedule("flashing off", 3, [this]() {
			setFlashingFrequency(0);
			});

		// caution: this is teleport which may break the CCD engine
		setRect(RectF(pos().x, pos().y + 1, 1, 1));
		_collider.adjust(0, -0.4f, 0, -1);
	}
	else
	{
		dynamic_cast<AnimatedSprite*>(_sprites[1]["big2small"])->reset();
		_transformSuper2Small = false;
		_super = false;
		_freezedPhysics = false;
		dynamic_cast<PlatformerGame*>(Game::instance())->freeze(false);
	}
}
