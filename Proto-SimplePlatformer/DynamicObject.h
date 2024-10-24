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
	class DynamicObject;
}

// DynamicObject class.
// - provides base class for all objects that actively resolve collisions
//   since DynamicObject accepts (and resolves) collision with any CollidableObject
class agp::DynamicObject : public CollidableObject
{
	public:

		DynamicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~DynamicObject() {}

		virtual std::string name() override { 
			return strprintf("DynamicObject[%d]", _id); 
		}
};