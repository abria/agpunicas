// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "DynamicObject.h"

namespace agp
{
	class Enemy;
}

// Enemy
// - base class for all enemies
class agp::Enemy : public DynamicObject
{
	protected:

	public:

		Enemy(Scene* scene, const RotatedRectF& rrect, Sprite* sprite, int layer = 0);

		// extends logic collision (+hurt by sword)
		virtual bool collision(CollidableObject* with, bool begin, const Vec2Df& normal) override;

		virtual std::string name() override { return strprintf("Enemy[%d]", _id); }
};