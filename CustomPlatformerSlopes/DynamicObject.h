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
class agp::DynamicObject : public CollidableObject
{
	protected:

		Direction _facingDir;
		Slope* _slopeContact = nullptr; // valid only during this movement step
		float _slopeSpeed = 1;
		bool _slopeSliding = false;
		float _slopeRequestedY = 0;
		PointF _movementStartPos;
		float _slopeTargetBottom = 0;
		void prepareMovement(float dt) override;
		void finishMovement(float dt) override;

	public:

		DynamicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~DynamicObject() {}

		bool collidableWith(CollidableObject* obj) override;
		RectF movementStartCollider() const { return _collider + _movementStartPos; }

		Direction facingDir() const { return _facingDir; }

		// overrides MovableObject's move (+facing dir)
		void move(Direction xDir) override;

		virtual std::string name() override { 
			return strprintf("DynamicObject[%d]", _id); 
		}
};
