// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Sword.h"
#include "Mario.h"
#include "SpriteFactory.h"
#include "Scene.h"
#include "AnimatedSprite.h"
#include "Enemy.h"

using namespace agp;

Sword::Sword(Mario* mario)
	: DynamicObject(mario->scene(), RectF(mario->pos().x, mario->pos().y, 2, 2), nullptr, mario->layer() - 1)
{
	_link = mario;
	_facingDir = _link->facingDir();
	_yGravityForce = 0;

	setSprite(SpriteFactory::instance()->get("link_sword"));
}

void Sword::update(float dt)
{
	_facingDir = _link->facingDir();

	// correct sprite transformation
	if (_facingDir == Direction::LEFT)
	{
		_angle = 0;
		_flip = SDL_FLIP_HORIZONTAL;
	}
	else if(_facingDir == Direction::RIGHT)
	{
		_angle = 0;
		_flip = SDL_FLIP_NONE;
	}

	// correct position based on Link
	if (_facingDir == Direction::RIGHT)
		_rect.pos = _link->pos() + PointF{ -2 / 16.0f, -7 / 16.0f };
	else if (_facingDir == Direction::LEFT)
		_rect.pos = _link->pos() + PointF{ -14 / 16.0f, -7 / 16.0f };

	// variable (animated) collider synced with animation
	defaultCollider();
	AnimatedSprite* animSprite = dynamic_cast<AnimatedSprite*>(_sprite);
	// tparam in [0, 1], reaches 1 in the middle of animation and goes back to 0
	float tparam = 1 - 2 * std::fabs(animSprite->currentTime() - 0.5f);
	if (_facingDir == Direction::RIGHT)
		_collider.adjust(0.5f, std::min(0.1f + tparam, 0.9f), std::min(-1.5f + tparam * 2, -0.2f), std::max(-tparam - 0.1f, -0.9f));
	else if (_facingDir == Direction::LEFT)
		_collider.adjust(std::max(1.5f - tparam * 2, 0.2f), std::min(0.1f + tparam, 0.9f), -0.5f, std::max(-tparam - 0.1f, -0.9f));
	
	DynamicObject::update(dt);
}

bool Sword::collidableWith(CollidableObject* obj)
{
	return dynamic_cast<Enemy*>(obj);
}

bool Sword::collision(CollidableObject* with, Direction fromDir)
{
	Enemy* enemy = dynamic_cast<Enemy*>(with);
	if (enemy)
		printf("Sword collided with %s\n", enemy->name().c_str());

	return true;
}