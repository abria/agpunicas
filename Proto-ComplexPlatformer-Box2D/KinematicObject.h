// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <map>
#include "RigidObject.h"

namespace agp
{
	class KinematicObject;
}

// KinematicObject class.
// - captures colliding DynamicObjects and corrects their movement
class agp::KinematicObject : public RigidObject
{
	protected:


	public:

		KinematicObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer = 0);
		virtual ~KinematicObject() {}

		virtual std::string name() override {
			return strprintf("KinematicObject[%d]", _id);
		}
};