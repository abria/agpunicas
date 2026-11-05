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
#include "sdlUtils.h"

using namespace agp;

constexpr float Slope::VerticalPenetration;
constexpr float Slope::IdleDownhillMultiplier;
constexpr float Slope::DownhillMultiplier;
constexpr float Slope::UphillMultiplier;

namespace
{
	// Validate before the base constructor registers the object in its scene.
	RectF slopeBounds(const LineF& line)
	{
		if (!std::isfinite(line.start.x) || !std::isfinite(line.start.y) ||
			!std::isfinite(line.end.x) || !std::isfinite(line.end.y) ||
			line.start.x == line.end.x || line.start.y == line.end.y)
			throw "Slope requires a finite inclined segment, neither horizontal nor vertical";
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

float Slope::supportY(const RectF& body) const
{
	float surface = surfaceY(body);
	// Taper penetration at both ends to meet adjacent flat ground continuously.
	float depth = std::min(VerticalPenetration,
		std::min(surface - rect().top(), rect().bottom() - surface));
	return surface + std::max(0.0f, depth);
}

float Slope::speedFactor(float vx) const
{
	if (vx == 0)
		return 1;
	bool uphill = _risesRight ? vx > 0 : vx < 0;
	return uphill ? UphillMultiplier : DownhillMultiplier;
}

float Slope::slideVelocity(float flatSpeed) const
{
	float speed = std::abs(flatSpeed) * IdleDownhillMultiplier;
	return _risesRight ? -speed : speed;
}

Slope::Motion Slope::prepareMovement(CollidableObject& body, Vec2Df& velocity,
	bool released, float minSpeed, float dt)
{
	Motion motion;
	if (!body.collidable() || velocity.y < 0 || dt <= 0)
		return motion; // jumping must detach from the surface

	const float contactTolerance = 0.01f; // scene units
	RectF curRect = body.sceneCollider();
	std::vector<Slope*> slopes;
	Slope* curSlope = nullptr;
	bool restingInValley = false;
	for (auto object : body.scene()->objects())
	{
		Slope* slope = object->to<Slope*>();
		if (!slope || slope->killed() || !slope->collidable() ||
			!slope->collidableWith(&body) || !body.collidableWith(slope))
			continue;
		slopes.push_back(slope);
		if (slope->overlapsX(curRect) &&
			std::abs(curRect.bottom() - slope->supportY(curRect)) <= contactTolerance)
		{
			// Opposite slopes can share a short flat support region after penetration.
			if (curSlope && curSlope->_risesRight != slope->_risesRight &&
				std::abs(curRect.bottom() - slope->supportY(curRect)) < 0.0001f &&
				std::abs(curRect.bottom() - curSlope->supportY(curRect)) < 0.0001f)
				restingInValley = true;
			if (!curSlope)
				curSlope = slope;
		}
	}

	if (curSlope)
	{
		// On release, brake uphill inertia before allowing passive sliding.
		if (released && (curSlope->_risesRight ? velocity.x > 0 : velocity.x < 0))
		{
			float gradient = curSlope->rect().size.y / curSlope->rect().size.x;
			float speed = std::max(0.0f, std::abs(velocity.x) - 60 * (1 + gradient) * dt);
			velocity.x = speed < minSpeed ? 0 : std::copysign(speed, velocity.x);
		}
		// Apply active multipliers only when force and motion agree.
		// Skidding keeps the flat-ground displacement until direction reverses.
		bool uphill = curSlope->_risesRight ? velocity.x > 0 : velocity.x < 0;
		motion.xFactor = released ? (uphill ? 1.0f : IdleDownhillMultiplier)
			: (body.skidding() ? 1.0f : curSlope->speedFactor(velocity.x));
	}
	velocity.x *= motion.xFactor;
	motion.sliding = curSlope && released && velocity.x == 0;
	if (motion.sliding)
		velocity.x = restingInValley ? 0 : curSlope->slideVelocity(body.maxSpeedX());

	RectF nextRect = curRect + velocity * dt;
	// Follow the descent before testing the opposite slope of a valley.
	if (curSlope)
		nextRect.pos.y += curSlope->supportY(nextRect) - nextRect.bottom();

	float contactY = INFINITY;
	for (auto slope : slopes)
	{
		if (!slope->overlapsX(nextRect))
			continue;
		float curSurfaceY = slope->supportY(curRect);
		float nextSurfaceY = slope->supportY(nextRect);
		bool onSurface = std::abs(curRect.bottom() - curSurfaceY) <= contactTolerance;
		// Accept support from above, keeping the highest reachable surface.
		if (curRect.bottom() <= curSurfaceY + contactTolerance &&
			(nextRect.bottom() >= nextSurfaceY || onSurface) && nextSurfaceY < contactY)
		{
			contactY = nextSurfaceY;
			motion.support = slope;
		}
	}
	// Leave at the endpoint height; finish confirms contact only with x overlap.
	if (!motion.support && curSlope)
	{
		motion.support = curSlope;
		contactY = curSlope->supportY(nextRect);
	}
	// Passive sliding stops at a valley bottom instead of climbing the other side.
	if (motion.sliding && motion.support && contactY < curRect.bottom())
	{
		velocity.x = 0;
		motion.support = curSlope;
		contactY = curRect.bottom();
	}
	if (motion.support)
		velocity.y = (contactY - curRect.bottom()) / dt;
	motion.velocityY = velocity.y;
	motion.bottom = curRect.bottom() + velocity.y * dt;
	return motion;
}

Slope* Slope::Motion::finishMovement(RectF& body, Vec2Df& velocity, const RectF& start, bool ceiling) const
{
	// Remove temporary slope speed, preserving any reduction caused by a wall.
	velocity.x = sliding ? 0 : velocity.x / xFactor;
	const float velTolerance = 0.01f;     // scene units/s, for CCD corrections
	const float contactTolerance = 0.01f; // scene units, for discrete corrections
	const float climbTolerance = 0.0001f;
	if (!support || support->killed() || velocity.y < velocityY - velTolerance)
		return nullptr;
	if (velocityY < 0 && (ceiling || body.bottom() > bottom + climbTolerance))
	{
		// A blocked climb must cancel BOTH axes, or the body enters the ramp.
		body = start;
		velocity = {0, 0};
		return nullptr;
	}
	if (velocityY > 0 && body.bottom() < bottom - contactTolerance)
		return nullptr; // a flat or moving platform intercepted the descent
	if (!support->overlapsX(body))
		return nullptr;

	// A wall may have shortened the move: align at the actual final x.
	body.pos.y += support->supportY(body) - body.bottom();
	velocity.y = 0; // grounded, including uphill: the next jump must remain possible
	return support;
}

bool Slope::joinsTop(CollidableObject* object) const
{
	if (!object->to<StaticObject*>() || object->compenetrable() ||
		object->to<Slope*>())
		return false;
	RectF slopeRect = sceneCollider(), block = object->sceneCollider();
	const float tolerance = 0.001f;
	bool sameTop = std::abs(block.top() - slopeRect.top()) < tolerance;
	bool atHighEnd = std::abs(surfaceY(block) - slopeRect.top()) < tolerance;
	bool adjacent = std::abs(block.left() - slopeRect.right()) < tolerance ||
		std::abs(block.right() - slopeRect.left()) < tolerance;
	return sameTop && atHighEnd && adjacent; // shared endpoint: also hand off one-way platforms to the slope
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
		DrawThickLine(renderer, points[0], points[1], _colliderPixelThickness, _colliderColor.adjustAlpha(255));
	}
}
