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
#include "CollidableObject.h"

namespace agp
{
	class Trigger;
}

// Trigger class
// -  performs a given task when collides with given object
class agp::Trigger : public CollidableObject
{
	private:

		std::function<void()> _task;
		CollidableObject* _watched;

	public:

		Trigger(Scene* scene, const RectF& rect, CollidableObject* watched, std::function<void()> task);

		// extends game logic (-physics, -collisions)
		virtual void update(float dt) override { RenderableObject::update(dt); }

		// extends logic collision (+trigger behavior)
		virtual bool collision(CollidableObject* with, Direction fromDir) override;

		virtual std::string name() override {
			return strprintf("Trigger[%d]", _id);
		}
};