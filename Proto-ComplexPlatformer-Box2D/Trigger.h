// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <functional>
#include "RigidObject.h"

namespace agp
{
	class Trigger;
}

// Trigger class
// -  performs a given task when collides with given object
class agp::Trigger : public RigidObject
{
	private:

		std::function<void()> _task;
		RigidObject* _watched;

	public:

		Trigger(GameScene* scene, const RotatedRectF& obb, RigidObject* watched, std::function<void()> task);

		// extends logic collision (+trigger behavior)
		virtual void collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB) override;

		virtual std::string name() override {
			return strprintf("Trigger[%d]", _id);
		}
};