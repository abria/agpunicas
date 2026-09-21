// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "RigidObject.h"

namespace agp
{
	class StaticObject;
}

// StaticObject class.
// - provides base class for all objects that do not generally update physics
//   nor detect/resolve collisions
class agp::StaticObject : public RigidObject
{
	protected:


	public:

		StaticObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer = 0);
		virtual ~StaticObject() {}

		// extends logic collision (+focus)
		virtual void collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB) override;

		virtual std::string name() override {
			return strprintf("StaticObject[%d]", _id);
		}
};