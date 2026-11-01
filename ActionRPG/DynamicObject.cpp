// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DynamicObject.h"
#include "StaticObject.h"
#include "Scene.h"
#include "collisionUtils.h"

using namespace agp;

DynamicObject::DynamicObject(Scene* scene, const RotatedRectF& rrect, Sprite* sprite, int layer) :
	CollidableObject(scene, rrect, sprite, layer)
{
	// dynamic objects are compenetrable vs. each other by default
	// (e.g. player vs. spanwable, collectibles vs. enemies, ...)
	// compenetration does not need to be resolved in these cases
	// since the collision is resolved "logically" by the collision method
	_compenetrable = true;

	_facingDir = Direction::DOWN;
}

void DynamicObject::move(Direction xDir, Direction yDir)
{
	if (xDir != Direction::NONE && xDir != _facingDir)
		_facingDir = xDir;
	if (yDir != Direction::NONE && yDir != _facingDir)
		_facingDir = yDir;

	CollidableObject::move(xDir, yDir);
}

void DynamicObject::resolveCollisions()
{
	if (!_collidable || _killed)
		return;

	Vec2Df correction;
	for (auto item : _scene->objects(sceneCollider().boundingRect()))
	{
		auto other = item->to<CollidableObject*>();
		Vec2Df normal;
		float depth;
		if (other && other != this && !other->killed() && other->collidable()
			&& collidableWith(other) && other->collidableWith(this)
			&& checkCollisionSAT(sceneCollider().verticesVec(), other->sceneCollider().verticesVec(), normal, depth))
		{
			// Keep the original hard response to walls and soft response to other bodies.
			if (other->to<StaticObject*>() && !other->compenetrable())
				correction -= normal * depth;
			else if (other->to<DynamicObject*>())
				correction -= normal * depth / 10.0f;
		}
	}
	setPos(pos() + correction);
}