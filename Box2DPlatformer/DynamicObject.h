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
#include <set>

namespace agp
{
	class StaticObject;
	class DynamicObject;
}

// DynamicObject class.
// - movement forces and impulses
// - velocity clipping
// - ground detection
class agp::DynamicObject : public RigidObject
{
	protected:

		float _xMoveForce;			// horizontal move (frame-by-frame) force
		float _yJumpImpulse;		// jump impulse
		float _xVelMax;				// maximum horizontal velocity (scene units / s)
		float _yVelMax;				// maximum vertical velocity
		Direction _xDir;			// commanded horizontal movement
		std::set<RigidObject*> _contactsWithGround;	// for ground detection

	public:

		DynamicObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer = 0);
		virtual ~DynamicObject() {}

		// overrides RigidObject's update (+velocity clipping)
		virtual void update(float dt) override;

		// override logic collision (+ground detection)
		virtual void collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB) override;

		// state queries
		bool skidding() const;
		bool onGround() const;

		// actions
		virtual void move(Direction dir);
		virtual void jump();

		virtual std::string name() override { 
			return strprintf("DynamicObject[%d]", _id); 
		}
};