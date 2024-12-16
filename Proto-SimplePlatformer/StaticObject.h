// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "CollidableObject.h"

namespace agp
{
	class StaticObject;
}

// StaticObject class.
// - provides base class for all objects that do not generally update physics
//   nor detect/resolve collisions
class agp::StaticObject : public CollidableObject
{
	protected:


	public:

		StaticObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~StaticObject() {}

		// extends game logic (-physics, -collisions)
		virtual void update(float dt) override { RenderableObject::update(dt); }

		virtual std::string name() override {
			return strprintf("StaticObject[%d]", _id);
		}

		virtual bool collision(CollidableObject* with, bool begin, Direction fromDir) {
			if(!begin)
				printf("%s vs. %s: %s\n", name().c_str(), with->name().c_str(), begin ? "BEGIN" : "end");
			return true;
		}
};