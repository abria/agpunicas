// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2023 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "StaticObject.h"

namespace agp
{
	class Slope;
}

// Slope class.
// - defines a static inclined segment, drawn as a triangle (positive width and height)
// - plans movement along slopes and checks support after ordinary collisions
// - keeps movement state in each body, never in the shared slope
class agp::Slope : public StaticObject
{
	protected:

		bool _risesRight;	// true = uphill when moving right

	public:

		// Gameplay tuning; speeds are horizontal, relative to movement on flat ground.
		static constexpr float VerticalPenetration = 0.25f; // maximum, in scene units
		static constexpr float IdleDownhillMultiplier = 1.0f; // uses the body's flat speed limit
		static constexpr float DownhillMultiplier = 1.3f;
		static constexpr float UphillMultiplier = 0.8f;
		static_assert(VerticalPenetration >= 0 && IdleDownhillMultiplier >= 0 &&
			DownhillMultiplier > 0 && UphillMultiplier > 0,
			"Slope requires nonnegative penetration/idle speed and positive active multipliers");

		// A body's plan for one step; ordinary collisions run between prepare and finish.
		struct Motion
		{
			Slope* support = nullptr;
			float xFactor = 1;
			bool sliding = false;
			float velocityY = 0, bottom = 0; // planned vertical speed and final foot height

			// Adjust scene collider and velocity; return confirmed support, if any.
			Slope* finishMovement(RectF& body, Vec2Df& velocity, const RectF& start, bool ceiling) const;
		};

		Slope(Scene* scene, const LineF& line, int layer = 1);

		// Query accepted slopes and constrain this step's velocity, without moving the body.
		static Motion prepareMovement(CollidableObject& body, Vec2Df& velocity,
			bool released, float minSpeed, float dt);
		bool joinsTop(CollidableObject* object) const; // flat continuation at the high endpoint

		// slope queries (scene coordinates, y points downwards)
		bool overlapsX(const RectF& body) const;
		float surfaceY(const RectF& body) const; // height at the uphill foot, clamped to the segment
		float supportY(const RectF& body) const; // foot height including allowed penetration
		float speedFactor(float vx) const;     // horizontal displacement multiplier, always positive
		float slideVelocity(float flatSpeed) const; // passive downhill speed, relative to the body's flat speed limit

		// disable generic shape tests: the bounding rect is not a solid block
		// prepareMovement/finishMovement resolve contact with the segment instead
		virtual bool sweep(const RectF&, const Vec2Df&, Vec2Df&, Vec2Df&, float&) const override { return false; }
		virtual bool overlap(const RectF&, Direction&, float&) const override { return false; }

		// extends rendering (+triangular shape)
		virtual void draw(SDL_Renderer* renderer, Transform camera) override;
};
