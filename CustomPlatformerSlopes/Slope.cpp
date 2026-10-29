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
#include <stdexcept>

using namespace agp;

namespace
{
	// Validate before the base constructor registers the object in its scene.
	RectF slopeBounds(const LineF& line)
	{
		if (!std::isfinite(line.start.x) || !std::isfinite(line.start.y) ||
			!std::isfinite(line.end.x) || !std::isfinite(line.end.y) ||
			line.start.x == line.end.x || line.start.y == line.end.y)
			throw std::runtime_error("Slope requires a finite inclined segment, neither horizontal nor vertical");
		return line.boundingRect(false);
	}
}

Slope::Slope(Scene* scene, const LineF& line, int layer) :
	StaticObject(scene, slopeBounds(line), layer)
{
	// y points downwards; the direction does not depend on endpoint order.
	_risesRight = line.start.x < line.end.x ? line.end.y < line.start.y : line.start.y < line.end.y;
	setColor({0, 0, 0, 0}); // collision-only surface, graphics belong to the scenery
}

bool Slope::overlapsX(const RectF& body) const
{
	// touching an endpoint without horizontal overlap is not support
	return body.right() > rect().left() && body.left() < rect().right();
}

float Slope::surfaceY(const RectF& body) const
{
	// sample at the uphill foot corner, where the surface is highest
	// placing the bottom here keeps the whole axis-aligned body above the slope
	float footX = _risesRight ? body.right() : body.left();

	// clamp the sample to the segment endpoints
	// a body straddling a slope and flat ground must use the endpoint height,
	// not the height obtained by extending the inclined line beyond the slope
	float x = std::max(rect().left(), std::min(footX, rect().right()));
	float t = (x - rect().left()) / rect().size.x; // 0 = left endpoint, 1 = right endpoint
	return _risesRight ? rect().bottom() - t * rect().size.y
					   : rect().top() + t * rect().size.y;
}

float Slope::speedFactor(float vx) const
{
	if (vx == 0)
		return 1;

	// gameplay rule: slow down uphill, speed up downhill
	// use actual velocity rather than input direction (e.g. while skidding)
	// the factor changes horizontal displacement, not stored acceleration
	float gradient = rect().size.y / rect().size.x; // |dy/dx|
	bool uphill = _risesRight ? vx > 0 : vx < 0;
	return uphill ? 1.0f / (1.0f + gradient) : 1.0f + gradient;
}

float Slope::slideVelocity() const
{
	// gameplay rule: downhill surface speed grows with gradient, up to 10 units/s
	// project onto x so steep ramps cannot produce excessive vertical speed
	float gradient = rect().size.y / rect().size.x;
	float speed = 5 * std::min(gradient, 2.0f) / std::sqrt(1.0f + gradient * gradient);
	return _risesRight ? -speed : speed;
}

void Slope::draw(SDL_Renderer* renderer, Transform camera)
{
	if (!_visible)
		return;

	Color color = _focused ? _focusColor : _color;
	_focused = false;

	// first two vertices define the inclined segment; the third closes the fill
	PointF points[] = {
		camera(_risesRight ? rect().bl() : rect().tl()),
		camera(_risesRight ? rect().tr() : rect().br()),
		camera(_risesRight ? rect().br() : rect().bl())
	};
	SDL_Vertex vertices[3]{};
	for (int i = 0; i < 3; i++)
	{
		vertices[i].position = points[i].toSDLf();
		vertices[i].color = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
	}
	SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);

	// only the inclined segment collides; the base and vertical side are visual
	GameScene* scene = dynamic_cast<GameScene*>(_scene);
	if (scene && scene->collidersVisible())
	{
		SDL_SetRenderDrawColor(renderer, _colliderColor.r, _colliderColor.g, _colliderColor.b, 255);
		SDL_RenderLine(renderer, points[0].x, points[0].y, points[1].x, points[1].y);
	}
}
