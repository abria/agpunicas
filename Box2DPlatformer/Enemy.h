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

		bool _smashable;
		bool _dying;

	public:

		Enemy(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer = 0);

		// overrides RigidObject's update (+dying animation)
		virtual void update(float dt) override;

		// actions
		virtual void hurt();

		// extends logic collision (+smashed, +hurt Player)
		virtual void collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB) override;

		virtual std::string name() override { return strprintf("Enemy[%d]", _id); }
};