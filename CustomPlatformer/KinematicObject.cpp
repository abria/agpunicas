// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "KinematicObject.h"
#include "DynamicObject.h"

using namespace agp;

KinematicObject::KinematicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	CollidableObject(scene, rect, sprite, layer)
{
	_compenetrable = false;
}

void KinematicObject::update(float dt)
{
	CollidableObject::update(dt);

	for (auto obj : _collidingItems)
	{
		// object "on" the platform -> object moves along with platform
		// NOTE: teleport is OK since we have disabled CCD
		if (obj.second == Direction::UP)
			obj.first->moveBy(_vel * dt);

		// object hits platform from the bottom -> object pos corrected along y 
		else if (obj.second == Direction::DOWN)
			obj.first->moveBy(Vec2Df(0, _vel.y * dt));

		// object hits platform from one side -> object pos corrected along x
		else
			obj.first->moveBy(Vec2Df(_vel.x * dt, 0));
	}
	_collidingItems.clear();
}

bool KinematicObject::collision(CollidableObject* with, bool begin, Direction fromDir)
{
	// disable CCD when 'with' is on (fromDir == UP) this kinetic object 
	if(fromDir == Direction::UP || !begin)
		with->setCCD(!begin);

	DynamicObject* dobj = with->to<DynamicObject*>();
	if (dobj)
	{
		_collidingItems[dobj] = fromDir;
		return true;
	}
	else
		return false;
}

bool KinematicObject::collidableWith(CollidableObject* obj)
{
	return obj->to<DynamicObject*>();
}