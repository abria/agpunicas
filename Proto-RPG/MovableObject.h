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
// - provides basic physics actions (e.g. move, jump, ...)
class agp::MovableObject : public RenderableObject
{
	protected:

		// physics parameters (expressed in scene units/s)
		Vec2Df _moveForce;		// acceleration due to movement
		Vec2Df _frictionForce;	// deceleration due to movement release (friction)
		Vec2Df _velMin;
		Vec2Df _velMax;
		virtual void defaultPhysics();

		// attributes
		Direction _xDir;		// commanded horizontal direction
		Direction _yDir;		// commanded horizontal direction
		Vec2Df _vel;			// current velocity
		Vec2Df _velPrev;		// velocity in the previous iteration

	public:

		MovableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~MovableObject() {}

		// getters / setters
		Vec2Df vel() const { return _vel; }
		virtual void velAdd(const Vec2Df& amount);
		virtual void velClip(float vx, float vy);
		virtual void moveBy(const Vec2Df& amount) { _rect.pos += amount; }
		virtual void teleport(const Vec2Df& newpos) { _rect.pos = newpos; }

		// actions
		virtual void move(Direction xDir, Direction yDir);
		
		// extends game logic (+physics)
		virtual void update(float dt) override;

		virtual std::string name() override {
			return strprintf("MovableObject[%d]", _id);
		}
};