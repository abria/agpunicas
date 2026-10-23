// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DynamicObject.h"
#include "Slope.h"
#include "Scene.h"
#include "Platform.h"

using namespace agp;

DynamicObject::DynamicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	CollidableObject(scene, rect, sprite, layer)
{
	// dynamic objects are compenetrable vs. each other by default
	// (e.g. player vs. spawnable, collectibles vs. enemies, ...)
	// compenetration does not need to be resolved in these cases
	// since the collision is resolved "logically" by the collision method
	_compenetrable = true;

	_facingDir = Direction::RIGHT;
	_movementStartPos = pos();
	_slope = nullptr;
	_x_vel_factor = 1;
	_slope_sliding = false;
	_slope_vel_y = 0;
	_slope_bottom = 0;
}

// overrides MovableObject's move (+facing dir)
void DynamicObject::move(Direction xDir)
{
	if (xDir != Direction::NONE && xDir != _facingDir)
		_facingDir = xDir;

	CollidableObject::move(xDir);
}

void DynamicObject::prepareMovement(float dt)
{
	_slope = nullptr;
	_x_vel_factor = 1;
	_slope_sliding = false;
	_movementStartPos = pos();
	if (!_collidable)
		return;

	// first constrain the movement to the slope surface, if any
	resolveSlopeCollisions(dt);

	// save the planned vertical movement before resolving other collisions
	// CCD changes velocity; discrete resolution changes position instead
	// finishMovement() needs both to decide whether the slope still supports us
	_slope_vel_y = _vel.y;
	_slope_bottom = sceneCollider().bottom() + _vel.y * dt;
}

void DynamicObject::resolveSlopeCollisions(float dt)
{
	// negative vertical velocity here means jumping, since finishMovement()
	// resets it after every confirmed slope contact, including uphill movement
	if (_vel.y < 0)
		return;

	RectF curRect = sceneCollider();
	Objects objects = _scene->objects();
	Slope* curSlope = nullptr;
	const float contactTolerance = 0.01f; // scene units, to keep contact despite float precision

	// find the slope supporting the body BEFORE the move
	// this contact determines the speed factor and how to follow a descent
	for (auto obj : objects)
	{
		Slope* slope = obj->to<Slope*>();
		if (slope && !slope->killed() && slope->collidable() &&
			slope->collidableWith(this) && collidableWith(slope) && slope->overlapsX(curRect) &&
			std::abs(curRect.bottom() - slope->surfaceY(curRect)) <= contactTolerance)
		{
			curSlope = slope;
			break;
		}
	}

	if (curSlope)
	{
		// releasing an uphill command should soon give way to passive sliding
		// brake the remaining input velocity before applying the slope factor
		// steeper ramps brake harder; clamp the speed so braking cannot reverse it
		if (_xDir == Direction::NONE && curSlope->speedFactor(_vel.x) < 1)
		{
			float gradient = curSlope->rect().size.y / curSlope->rect().size.x;
			float releaseDeceleration = 60.0f * (1.0f + gradient);
			float speed = std::max(0.0f, std::abs(_vel.x) - releaseDeceleration * dt);
			_vel.x = speed < _xVelMin ? 0 : std::copysign(speed, _vel.x);
		}
		_x_vel_factor = curSlope->speedFactor(_vel.x);
	}

	// apply the slope factor to this move only (removed in finishMovement())
	_vel.x *= _x_vel_factor;

	// passive sliding supplies a displacement with neither a command nor residual inertia
	// a movement command takes precedence, allowing uphill movement from rest
	// finishMovement() removes this temporary velocity instead of storing it
	_slope_sliding = curSlope && _xDir == Direction::NONE && _vel.x == 0;
	if (_slope_sliding)
		_vel.x = curSlope->slideVelocity();
	RectF nextRect = curRect + _vel * dt;

	// predict movement along the current slope before testing other slopes
	// gravity alone may not keep up with a descent; in a valley we must test
	// the descending path against the opposite slope, not the free-fall path
	if (curSlope)
		nextRect.pos.y += curSlope->surfaceY(nextRect) - nextRect.bottom();

	// find the slope supporting the body AFTER the move
	// y points downwards: the highest supporting surface has the smallest y
	float contactY = INFINITY;
	for (auto obj : objects)
	{
		Slope* slope = obj->to<Slope*>();
		if (!slope || slope->killed() || !slope->collidable() ||
			!slope->collidableWith(this) || !collidableWith(slope) || !slope->overlapsX(nextRect))
			continue;

		float curSurfaceY = slope->surfaceY(curRect);
		float nextSurfaceY = slope->surfaceY(nextRect);
		bool onSurface = std::abs(curRect.bottom() - curSurfaceY) <= contactTolerance;

		// accept contact only from above: the segment can be crossed from below
		// either the feet reach the surface, or contact already exists and must
		// be maintained downhill even when gravity would leave the body above it
		if (curRect.bottom() <= curSurfaceY + contactTolerance &&
			(nextRect.bottom() >= nextSurfaceY || onSurface) && nextSurfaceY < contactY)
		{
			contactY = nextSurfaceY;
			_slope = slope;
		}
	}

	// when leaving a slope, finish this step at the endpoint height
	// surfaceY() clamps to the endpoints, allowing a transition to flat ground
	// finishMovement() will record contact only if horizontal overlap remains
	if (!_slope && curSlope)
	{
		_slope = curSlope;
		contactY = curSlope->surfaceY(nextRect);
	}

	// passive sliding cannot climb the opposite side of a valley
	// cancel the step when the selected surface is higher than the current feet,
	// otherwise switching support would reverse the slide at every iteration
	if (_slope_sliding && _slope && contactY < curRect.bottom())
	{
		_vel.x = 0;
		_slope = curSlope;
		contactY = curRect.bottom();
	}

	// convert the vertical displacement into a velocity for this iteration
	// CollidableObject will use it for collision resolution and move by _vel * dt
	// finishMovement() resets it to zero AFTER the move if support is confirmed
	if (_slope)
		_vel.y = (contactY - curRect.bottom()) / dt;
}

void DynamicObject::finishMovement(float dt)
{
	// remove the slope factor from horizontal velocity
	// divide the resolved velocity, so any reduction due to a wall is preserved
	// passive sliding is removed entirely: storing it would feed the friction
	// calculation next time, possibly creating a velocity uphill after a wall
	_vel.x = _slope_sliding ? 0 : _vel.x / _x_vel_factor;
	if (!_slope)
		return;

	const float velTolerance = 0.01f;     // scene units/s, before/after collision resolution
	const float contactTolerance = 0.01f; // scene units, platform vs. planned slope height
	const float climbTolerance = 0.0001f; // scene units, upward displacement blocked by a ceiling

	// callbacks may disable physics (death/transformation) or remove the slope
	// a smaller vy may also mean a bounce or a descent stopped by CCD
	// discard the contact so slope alignment cannot overwrite that response
	if (!_collidable || _freezedPhysics || _slope->killed() ||
		_vel.y < _slope_vel_y - velTolerance)
	{
		_slope = nullptr;
		return;
	}

	bool ceiling = false;
	for (const Vec2Df& axis : _collisionAxes)
		ceiling = ceiling || axis == dir2vec(Direction::UP);
	float bottom = sceneCollider().bottom();
	if (_slope_vel_y < 0 && (ceiling || bottom > _slope_bottom + climbTolerance))
	{
		// a ceiling stopped the climb: horizontal and vertical movement are coupled
		// cancel BOTH, otherwise moving horizontally would enter the slope
		// the body has already moved, so restore its position at the start of the step
		// discard the contact too, preventing later alignment through the ceiling
		setPos(_movementStartPos);
		_vel = { 0, 0 };
		_slope = nullptr;
		return;
	}
	if (_slope_vel_y > 0 && bottom < _slope_bottom - contactTolerance)
	{
		// a flat or moving platform stopped the descent before reaching the slope
		// keep the position resolved by collisions and discard the slope contact,
		// otherwise alignment would move the body down through the platform
		_slope = nullptr;
		return;
	}

	RectF curRect = sceneCollider();
	if (_slope->overlapsX(curRect))
	{
		// align the body with the surface at the final horizontal position
		// a wall may have shortened the horizontal move after the slope was tested
		// use setPos() so the scene also updates its spatial index
		setPos(pos() + PointF(0, _slope->surfaceY(curRect) - curRect.bottom()));

		// restore the vertical state of an object resting on the ground
		// _vel.y was only needed to move along the slope, not to jump or fall
		// leaving it negative uphill would prevent jump() and detach us next time;
		// leaving it positive downhill would make midair() report an airborne body
		// this is needed on ANY confirmed slope contact, independently of walls
		_vel.y = 0;

		// register support for discrete midair() and for contact-end detection
		// slopes are excluded from both generic shape tests (see Slope)
		_collisions.push_back(_slope);
		_collisionAxes.push_back(dir2vec(Direction::DOWN));
		_collisionDepths.push_back(0);

		// notify logic collisions after position and velocity have been resolved
		_slope->collision(this, true, Direction::UP);
		collision(_slope, true, Direction::DOWN);
	}

	// contact is computed again next iteration (the body may jump or leave)
	// clearing this temporary pointer does not erase the recorded collision
	_slope = nullptr;
}

bool DynamicObject::collidableWith(CollidableObject* obj)
{
	// a static block can continue the slope at its high endpoint
	// its vertical side is an internal seam, not a wall: exclude the block
	// from collision tests during this slope contact to allow the body onto its top
	// one-way platforms and compenetrable objects keep their own collision rules
	if (_slope && obj->to<StaticObject*>() &&
		!obj->compenetrable() && !obj->to<Platform*>() && !obj->to<Slope*>())
	{
		RectF slopeRect = _slope->sceneCollider();
		RectF objRect = obj->sceneCollider();
		const float joinTolerance = 0.001f; // scene units, for coincident endpoints

		bool sameTop = std::abs(objRect.top() - slopeRect.top()) < joinTolerance;
		bool atHighEnd = std::abs(_slope->surfaceY(objRect) - slopeRect.top()) < joinTolerance;
		bool adjacent = std::abs(objRect.left() - slopeRect.right()) < joinTolerance ||
						std::abs(objRect.right() - slopeRect.left()) < joinTolerance;
		if (sameTop && atHighEnd && adjacent)
			return false;
	}
	return CollidableObject::collidableWith(obj);
}
