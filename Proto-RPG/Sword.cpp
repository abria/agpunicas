// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Sword.h"
#include "Link.h"
#include "SpriteFactory.h"
#include "Scene.h"
#include "AnimatedSprite.h"

using namespace agp;

Sword::Sword(Link* link)
	: DynamicObject(link->scene(), RectF(link->pos().x, link->pos().y, 2, 2), nullptr, link->layer() - 1)
{
	_link = link;
	_facingDir = link->facingDir();

	setSprite(SpriteFactory::instance()->get("link_sword_RIGHT"));
}

void Sword::update(float dt)
{
	_facingDir = _link->facingDir();

	// correct sprite transformation
	if (_facingDir == Direction::UP)
	{
		_angle = 90;
		_flip = SDL_FLIP_VERTICAL;
	}
	else if (_facingDir == Direction::DOWN)
	{
		_angle = 90;
		_flip = SDL_FLIP_HORIZONTAL;
	}
	else if (_facingDir == Direction::LEFT)
	{
		_angle = 0;
		_flip = SDL_FLIP_HORIZONTAL;
	}
	else
	{
		_angle = 0;
		_flip = SDL_FLIP_NONE;
	}

	// correct position based on Link
	if (_facingDir == Direction::RIGHT)
		_rect.pos = _link->pos() + PointF{ 5 / 16.0f, -3 / 16.0f };
	else if (_facingDir == Direction::LEFT)
		_rect.pos = _link->pos() + PointF{ -21 / 16.0f, -3 / 16.0f };
	else if (_facingDir == Direction::UP)
		_rect.pos = _link->pos() + PointF{ -11 / 16.0f, -16 / 16.0f };
	else if (_facingDir == Direction::DOWN)
		_rect.pos = _link->pos() + PointF{ -5 / 16.0f, 8 / 16.0f };
	else
		_rect.pos = _link->pos();

	// variable (animated) collider synced with animation
	defaultCollider();
	AnimatedSprite* animSprite = dynamic_cast<AnimatedSprite*>(_sprite);
	// tparam in [0, 1], reaches 1 in the middle of animation and goes back to 0
	float tparam = 1 - 2 * std::fabsf(animSprite->currentTime() - 0.5f);
	if (_facingDir == Direction::RIGHT)
		_collider.adjust(0, std::min(0.1f + tparam, 0.9f), std::min(- 2 + tparam * 2, -0.2f), std::max(- tparam - 0.1f, -0.9f));
	else if(_facingDir == Direction::LEFT)
		_collider.adjust(std::max(2-tparam * 2, 0.2f), std::min(0.1f + tparam, 0.9f), 0, std::max(-tparam - 0.1f, -0.9f));
	else if (_facingDir == Direction::DOWN)
		_collider.adjust(std::min(0.1f + tparam, 0.9f), 0, std::max(-tparam - 0.1f, -0.9f), std::min(-2 + tparam * 2, -0.2f));
	else if (_facingDir == Direction::UP)
		_collider.adjust(std::min(0.1f + tparam, 0.9f), std::max(2 - tparam * 2, 0.2f), std::max(-tparam - 0.1f, -0.9f), 0);

	DynamicObject::update(dt);
}