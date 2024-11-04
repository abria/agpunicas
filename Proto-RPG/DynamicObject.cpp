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

	_facingDir = Direction::DOWN;
}

// overrides MovableObject's move (+facing dir)
void DynamicObject::move(Direction xDir, Direction yDir)
{
	if (xDir != Direction::NONE && xDir != _facingDir)
		_facingDir = xDir;
	if (yDir != Direction::NONE && yDir != _facingDir)
		_facingDir = yDir;

	//if(name().compare(0, 7, "Soldier") == 0)
	//	printf("move %s %s\n", dir2str(xDir).c_str(), dir2str(yDir).c_str());
	CollidableObject::move(xDir, yDir);
}