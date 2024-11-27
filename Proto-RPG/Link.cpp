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
#include "Scene.h"
#include "Sword.h"

using namespace agp;

Link::Link(Scene* scene, const PointF& pos)
	: DynamicObject(scene, RectF( pos.x, pos.y, 1, 1.5f ), nullptr)
{
	_collider.size.x -= 0.2f;
	_collider.size.y = 0.9f;
	_collider.center.y = 0.8f;

	_facingDir = Direction::DOWN;
	_walking = false;
	_attacking = false;
	_layer = 2;
	_sword = nullptr;

	// animations
	for (int i = 0; i < 4; i++)
	{
		_sprites[i]["stand"] = SpriteFactory::instance()->get(std::string("link_stand_" + dir2str(Direction(i))));
		_sprites[i]["walk"] = SpriteFactory::instance()->get(std::string("link_walk_" + dir2str(Direction(i))));
		_sprites[i]["attack"] = SpriteFactory::instance()->get(std::string("link_attack_" + dir2str(Direction(i))));
	}
	setSprite(_sprites[int(_facingDir)]["stand"]);

	// decorations
	_shadow = new RenderableObject(scene, _rect, SpriteFactory::instance()->get(std::string("link_shadow")), _layer - 1);
}

void Link::update(float dt)
{
	// physics
	DynamicObject::update(dt);

	// state logic
	_walking = _vel.mag() != 0;

	// animations
	if(_attacking)
		setSprite(_sprites[int(_facingDir)]["attack"]);
	else if(_walking)
		setSprite(_sprites[int(_facingDir)]["walk"]);
	else
		setSprite(_sprites[int(_facingDir)]["stand"]);

	// decorations
	_shadow->setPos(_rect.pos +Vec2Df(dir2vec(_facingDir).x * (2.0f / _scene->pixelUnitSize().x), dir2vec(_facingDir).y == 0? 1.0f / _scene->pixelUnitSize().y : 0));

	// x-mirroring
	if (_facingDir == Direction::LEFT)
		_flip = SDL_FLIP_HORIZONTAL;
	else
		_flip = SDL_FLIP_NONE;
}

void Link::attack()
{
	Audio::instance()->playSound("fighter sword 1");

	if (_attacking)
	{
		_sprite->reset();
		if (_sword)
			_scene->killObject(_sword);
	}

	_sword = new Sword(this);
	_attacking = true;
	schedule("attack_off", dynamic_cast<AnimatedSprite*>(_sprites[int(_facingDir)]["attack"])->duration(), [this]() 
		{
			_attacking = false; 
			_scene->killObject(_sword);
			_sword = nullptr;
		});
}

void Link::die()
{
	// to be implemented
}

void Link::hurt()
{
	// to be implemented
}

void Link::setPos(const PointF& newPos)
{
	printf("Link teleported to %s\n", newPos.str().c_str());
	DynamicObject::setPos(newPos);
}