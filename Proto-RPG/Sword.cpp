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
	AnimatedSprite* animSprite = dynamic_cast<AnimatedSprite*>(_sprite);
	if (_facingDir == Direction::RIGHT)
		_collider = RotatedRectF(0.7f, 1, 1.5f, 0.2f, PI / 2 - animSprite->currentTime() * PI);
	else if (_facingDir == Direction::LEFT)
		_collider = RotatedRectF(2-0.7f, 1, 1.5f, 0.2f, -PI / 2 + animSprite->currentTime() * PI);
	else if (_facingDir == Direction::DOWN)
		_collider = RotatedRectF(0.8f, 0.8f, 1.5f, 0.2f, animSprite->currentTime() * PI);
	else if (_facingDir == Direction::UP)
		_collider = RotatedRectF(1.2f, 1.2f, 1.5f, 0.2f, animSprite->currentTime() * PI);

	if (animSprite->currentTime() > 0.5f && _layer < _link->layer())
		_scene->changeLayerObject(this, _link->layer() + 1);

	DynamicObject::update(dt);
}