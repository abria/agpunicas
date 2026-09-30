// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2023 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Slope.h"
#include "GameScene.h"

using namespace agp;

bool Slope::overlapsX(const RectF& body) const
{
	return body.right() > rect().left() && body.left() < rect().right();
}

float Slope::surfaceY(const RectF& body) const
{
	// use the uphill foot corner to avoid compenetration
	// clamp to the slope endpoints for transitions to flat platforms
	const float footX = _risesRight ? body.right() : body.left();
	const float x = std::max(rect().left(), std::min(footX, rect().right()));
	const float t = (x - rect().left()) / rect().size.x;
	return _risesRight ? rect().bottom() - t * rect().size.y
					   : rect().top() + t * rect().size.y;
}

float Slope::speedFactor(float vx) const
{
	const float slope = rect().size.y / rect().size.x; // |dy/dx|
	const bool uphill = _risesRight ? vx > 0 : vx < 0;
	if (vx == 0) return 1;
	return uphill ? 1.0f / (1.0f + slope) : 1.0f + slope;
}

float Slope::slideVelocity() const
{
	const float slope = rect().size.y / rect().size.x;
	// One scene unit/s per unit of gradient, capped at two along the surface.
	// Project onto x so steep ramps cannot produce excessive vertical speed.
	const float speed = 5*std::min(slope, 2.0f) / std::sqrt(1.0f + slope * slope);
	return _risesRight ? -speed : speed;
}

bool Slope::sweep(const RectF& body, const Vec2Df& movement,
						Vec2Df& point, Vec2Df& normal, float& time) const
{
	if (!StaticObject::sweep(body, movement, point, normal, time))
		return false;
	// DynamicObject handles the slope surface
	// only the bottom and the high vertical side use the AABB test
	return normal.y > 0 || (_risesRight ? normal.x > 0 : normal.x < 0);
}

void Slope::draw(SDL_Renderer* renderer, Transform camera)
{
	if (!_visible) return;
	const Color color = _focused ? _focusColor : _color;
	_focused = false;
	const PointF points[] = {
		camera(_risesRight ? rect().bl() : rect().tl()),
		camera(_risesRight ? rect().tr() : rect().br()),
		camera(_risesRight ? rect().br() : rect().bl())
	};
	SDL_Vertex vertices[3]{};
	for (int i = 0; i < 3; ++i)
	{
		vertices[i].position = points[i].toSDLf();
		vertices[i].color = {color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f};
	}
	SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);
	auto* scene = dynamic_cast<GameScene*>(_scene);
	if (scene && scene->collidersVisible())
	{
		SDL_SetRenderDrawColor(renderer, _colliderColor.r, _colliderColor.g, _colliderColor.b, 255);
		for (int i = 0; i < 3; ++i)
			SDL_RenderLine(renderer, points[i].x, points[i].y, points[(i+1)%3].x, points[(i+1)%3].y);
	}
}

// Discrete collision mode is used while riding kinematic platforms.
bool Slope::overlap(const RectF& body, Direction& axis, float& depth) const
{
	// The empty upper half of the bounding box is not solid.
	if (body.bottom() <= surfaceY(body) + 0.001f)
		return false;
	if (!StaticObject::overlap(body, axis, depth))
		return false;
	return axis == Direction::UP || (_risesRight ? axis == Direction::LEFT : axis == Direction::RIGHT);
}
