// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Trigger.h"

using namespace agp;

Trigger::Trigger(Scene* scene, const RotatedRectF& rrect, CollidableObject* watched, std::function<void()> task) :
	CollidableObject(scene, rrect, nullptr)
{
	_task = task;
	_watched = watched;
	_compenetrable = true;
}

// extends logic collision (+trigger behavior)
bool Trigger::collision(CollidableObject* with, bool begin, const Vec2Df& normal)
{
	if (with == _watched && begin)
	{
		_task();
		return true;
	}
	else
		return false;
}