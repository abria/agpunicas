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
#include "KinematicObject.h"

namespace agp
{
	class Gear;
}

// KinematicObject class.
// - captures colliding DynamicObjects and corrects their movement
class agp::Gear : public KinematicObject
{
	protected:


	public:

		Gear(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer = 0);
		virtual ~Gear() {}

		virtual std::string name() override {
			return strprintf("Gear[%d]", _id);
		}
};