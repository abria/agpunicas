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
// - defines a static inclined segment, drawn as a triangle
// - provides surface height and slope-dependent speed
class agp::Slope : public StaticObject
{
	protected:

		bool _risesRight;	// true = uphill when moving right

	public:

		Slope(Scene* scene, const RectF& rect, const Color& color, bool risesRight, int layer = 0)
			: StaticObject(scene, rect, nullptr, layer), _risesRight(risesRight) { setColor(color); }

		// slope queries
		bool overlapsX(const RectF& body) const;
		float surfaceY(const RectF& body) const;
		float speedFactor(float vx) const;
		float slideVelocity() const; // horizontal component of passive downhill motion

		// DynamicObject handles the segment; its bounding box is not solid.
		bool sweep(const RectF&, const Vec2Df&, Vec2Df&, Vec2Df&, float&) const override { return false; }
		bool overlap(const RectF&, Direction&, float&) const override { return false; }

		// extends rendering (+triangular shape)
		void draw(SDL_Renderer* renderer, Transform camera) override;
};
