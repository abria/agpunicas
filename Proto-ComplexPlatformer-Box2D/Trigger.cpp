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

Trigger::Trigger(GameScene* scene, const RotatedRectF& obb, RigidObject* watched, std::function<void()> task) :
	RigidObject(scene, obb, nullptr, b2BodyType::b2_staticBody)
{
	_task = task;
	_watched = watched;

	// sensor box shape
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.isSensor = true;
	shapeDef.userData = this;
	b2Polygon boxDef = b2MakeOffsetBox(_obb.size.x / 2, _obb.size.y / 2, {0, 0 }, { 1,0 });
}

// extends logic collision (+trigger behavior)
void Trigger::collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB)
{
	RigidObject::collision(with, begin, normal, shapeA, shapeB);

	if (with == _watched && begin)
		_task();
}