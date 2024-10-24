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
		// WARNING: this teleport may cause collisions with other objects to be missed!
		if (obj.second == Direction::UP)
			obj.first->moveBy(_vel * dt);
			// the fix below fixes the compenetration issue due to float precision
			// but generates spurious state transitions to 'falling' states
			//obj.first->moveBy(_vel * dt - Vec2Df(0, 0.001f * dt));

		// object hits platform from the bottom -> object pos corrected along y 
		else if (obj.second == Direction::DOWN)
			obj.first->moveBy(Vec2Df(0, _vel.y * dt));

		// object hits platform from one side -> object pos corrected along x
		else
			obj.first->moveBy(Vec2Df(_vel.x * dt, 0));
	}
	_collidingItems.clear();
}

bool KinematicObject::collision(CollidableObject* with, Direction fromDir)
{
	_focused = true;

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