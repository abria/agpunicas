// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DynamicObject.h"

using namespace agp;

DynamicObject::DynamicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	CollidableObject(scene, rect, sprite, layer)
{
	// dynamic objects are compenetrable vs. each other by default
	// (e.g. player vs. spanwable, collectibles vs. enemies, ...)
	// compenetration does not need to be resolved in these cases
	// since the collision is resolved "logically" by the collision method
	_compenetrable = true;

	_facingDir = Direction::RIGHT;
}

// overrides MovableObject's move (+facing dir)
void DynamicObject::move(Direction xDir)
{
	if (xDir != Direction::NONE && xDir != _facingDir)
		_facingDir = xDir;

	CollidableObject::move(xDir);
}