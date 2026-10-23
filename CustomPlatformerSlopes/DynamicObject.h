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
	class Slope;
}

// DynamicObject class.
// - provides base class for all objects that actively resolve collisions
//   since DynamicObject accepts (and resolves) collision with any CollidableObject
// - constrains movement to slope surfaces before CCD or discrete collision resolution
// - restores ground-contact velocity after moving along a slope
class agp::DynamicObject : public CollidableObject
{
	protected:

		Direction _facingDir;
		PointF _movementStartPos; // also used by one-way platforms

		// slope attributes (recomputed at each movement step)
		Slope* _slope;            // contact selected before the move, cleared after it
		float _x_vel_factor;      // horizontal movement factor (1 = no change)
		bool _slope_sliding;      // passive sliding, removed from stored velocity
		float _slope_vel_y;       // planned vertical velocity before other collisions
		float _slope_bottom;      // planned bottom coordinate before other collisions

		// collision detection/resolution (shared by CCD and discrete movement)
		virtual void prepareMovement(float dt) override;
		void resolveSlopeCollisions(float dt);
		virtual void finishMovement(float dt) override;

	public:

		DynamicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~DynamicObject() {}

		// defines acceptable collisions (+slope-to-platform transitions)
		virtual bool collidableWith(CollidableObject* obj) override;

		// collider before the move (also during CCD broad-phase prediction)
		RectF movementStartCollider() const { return _collider + _movementStartPos; }

		Direction facingDir() const { return _facingDir; }

		// overrides MovableObject's move (+facing dir)
		void move(Direction xDir) override;

		virtual std::string name() override { 
			return strprintf("DynamicObject[%d]", _id); 
		}
};
