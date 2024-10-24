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
#include <map>

namespace agp
{
	class DynamicObject;
	class KinematicObject;
}

// KinematicObject class.
// - captures colliding DynamicObjects and corrects their movement
class agp::KinematicObject : public CollidableObject
{
	protected:

		std::map<DynamicObject*, Direction> _collidingItems;

	public:

		KinematicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~KinematicObject() {}

		// extends game logic (+moving colliding dynamic objects)
		virtual void update(float dt) override;

		// extends logic collision (+capturing colliding dynamic objects)
		virtual bool collision(CollidableObject* with, Direction fromDir) override;

		// accepts only collisions with DynamicObject
		virtual bool collidableWith(CollidableObject* obj) override;

		virtual std::string name() override {
			return strprintf("KinematicObject[%d]", _id);
		}
};