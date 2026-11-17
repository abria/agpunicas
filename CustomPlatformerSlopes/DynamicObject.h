// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Slope.h"

namespace agp
{
	class DynamicObject;
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

		Slope::Motion _slopeMotion; // per-body plan, recomputed at each movement step

		// Extend ordinary collision movement with slope planning and finalization.
		virtual void resolveMovement(float dt) override;
		void prepareMovement(float dt);
		void finishMovement(float dt);

	public:

		DynamicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~DynamicObject() {}

		// defines acceptable collisions (+slope-to-platform transitions)
		virtual bool collidableWith(CollidableObject* obj) override;

		Direction facingDir() const { return _facingDir; }
		const Slope::Motion& motion() const { return _slopeMotion; }

		// overrides MovableObject's move (+facing dir)
		void move(Direction xDir) override;

		virtual std::string name() override { 
			return strprintf("DynamicObject[%d]", _id); 
		}
};
