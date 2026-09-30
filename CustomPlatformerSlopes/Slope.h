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
// - defines a static triangular collider
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

		// extends collision detection (+bottom and high vertical side)
		bool sweep(const RectF& body, const Vec2Df& movement,
			Vec2Df& point, Vec2Df& normal, float& time) const override;

		bool overlap(const RectF& body, Direction& axis, float& depth) const override;

		// extends rendering (+triangular shape)
		void draw(SDL_Renderer* renderer, Transform camera) override;
};
