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
// - provides surface height and slope-dependent horizontal speed
// - leaves segment collision resolution to DynamicObject
class agp::Slope : public StaticObject
{
	protected:

		bool _risesRight;	// true = uphill when moving right

	public:

		Slope(Scene* scene, const LineF& line, int layer = 0);

		// slope queries (scene coordinates, y points downwards)
		bool overlapsX(const RectF& body) const;
		float surfaceY(const RectF& body) const; // height at the uphill foot, clamped to the segment
		float speedFactor(float vx) const;     // horizontal displacement multiplier, always positive
		float slideVelocity() const; // horizontal component of passive downhill motion

		// disable generic shape tests: the bounding rect is not a solid block
		// DynamicObject resolves contact with the inclined segment instead
		virtual bool sweep(const RectF&, const Vec2Df&, Vec2Df&, Vec2Df&, float&) const override { return false; }
		virtual bool overlap(const RectF&, Direction&, float&) const override { return false; }

		// extends rendering (+triangular shape)
		virtual void draw(SDL_Renderer* renderer, Transform camera) override;
};
