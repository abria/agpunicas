// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "RenderableObject.h"

namespace agp
{
	class Scene;
	class MovableObject;
	class Sprite;
}

// MovableObject class.
// - implements free physics (no collisions)
// - provides physics state queries (falling, midair, ...)
// - provides basic physics actions (e.g. move, jump, ...)
class agp::MovableObject : public RenderableObject
{
	protected:

		// physics parameters (expressed in scene units/s)
		float _yGravityForce;	// vertical acceleration due to gravity
		float _xMoveForce;		// horizontal acceleration due to movement
		float _xFrictionForce;	// horizontal deceleration due to movement release
		float _xSkiddingForce;	// horizontal deceleration due to skidding (movement change)
		float _xVelMax;			// maximum horizontal velocity
		float _xVelMin;			// minimum horizontal velocity
		float _yVelMax;			// maximum vertical velocity
		float _yVelMin;			// minimum vertical velocity
		float _yJumpImpulse;	// initial vertical velocity when jumping
		virtual void defaultPhysics();

		// attributes
		Direction _xDir;		// current horizontal movement direction
		Vec2Df _vel;			// current velocity
		Vec2Df _prevVel;		// velocity in the previous iteration

	public:

		MovableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~MovableObject() {}

		// getters / setters
		Vec2Df vel() const { return _vel; }
		void velAdd(Vec2Df amount);
		void velClip(float vx, float vy);
		void setVelY(float vy) { _vel.y = vy; }
		void moveBy(Vec2Df amount) { _rect.pos += amount; }

		// state queries
		virtual bool skidding() const;
		virtual bool grounded() const;
		virtual bool falling() const;
		virtual bool midair() const;

		// actions
		virtual void move(Direction dir);
		virtual void jump();
		
		// extends game logic (+physics)
		virtual void update(float dt) override;

		virtual std::string name() override {
			return strprintf("MovableObject[%d]", _id);
		}
};