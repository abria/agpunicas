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
	// (e.g. player vs. spanwable, collectibles vs. enemies, ...)
	// compenetration does not need to be resolved in these cases
	// since the collision is resolved "logically" by the collision method
	_compenetrable = true;

	_facingDir = Direction::RIGHT;
	_movementStartPos = pos();
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
	_slopeContact = nullptr;
	_slopeSpeed = 1;
	_slopeSliding = false;
	_movementStartPos = pos();
	if (!_collidable)
		return;
	const RectF before = sceneCollider();
	Slope* support = nullptr;
	const float tolerance = 0.01f;
	const auto objects = _scene->objects();

	// find slope support only when not jumping
	if (_vel.y >= 0)
	{
		for (auto* object : objects)
		{
			auto* slope = object->to<Slope*>();
			if (slope && !slope->killed() && slope->collidable() && slope->collidableWith(this) && collidableWith(slope) && slope->overlapsX(before) &&
				std::abs(before.bottom() - slope->surfaceY(before)) <= tolerance)
			{
				support = slope;
				break;
			}
		}
		if (support)
		{
			// Releasing the uphill command should quickly give way to sliding.
			// Keep a short coast, with stronger braking on steeper ramps.
			if (_xDir == Direction::NONE && support->speedFactor(_vel.x) < 1)
			{
				const float gradient = support->rect().size.y / support->rect().size.x;
				const float releaseDeceleration = 60.0f * (1.0f + gradient);
				float speed = std::max(0.0f, std::abs(_vel.x) - releaseDeceleration * dt);
				_vel.x = speed < _xVelMin ? 0 : std::copysign(speed, _vel.x);
			}
			_slopeSpeed = support->speedFactor(_vel.x);
		}
		// scale movement for this step; restore horizontal velocity after the move
		_vel.x *= _slopeSpeed;
		// At rest, gravity produces a gentle downhill drift. A movement command
		// takes precedence so even a steep ramp can be climbed from a standstill.
		_slopeSliding = support && _xDir == Direction::NONE && _vel.x == 0;
		if (_slopeSliding)
			_vel.x = support->slideVelocity();
		RectF next = before + _vel * dt;
		// follow the current slope before testing other slopes (e.g. valleys)
		if (support)
			next.pos.y += support->surfaceY(next) - next.bottom();
		float surface = INFINITY;
		for (auto* object : objects)
		{
			auto* slope = object->to<Slope*>();
			if (!slope || slope->killed() || !slope->collidable() || !slope->collidableWith(this) || !collidableWith(slope) || !slope->overlapsX(next)) continue;
			const float oldY = slope->surfaceY(before);
			const float newY = slope->surfaceY(next);
			const bool wasOnSurface = std::abs(before.bottom() - oldY) <= tolerance;
			if (before.bottom() <= oldY + tolerance &&
				(next.bottom() >= newY || wasOnSurface) && newY < surface)
			{
				surface = newY;
				_slopeContact = slope;
			}
		}
		// keep support up to the slope endpoint
		if (!_slopeContact && support)
		{
			_slopeContact = support;
			surface = support->surfaceY(next);
		}
		if (_slopeSliding && _slopeContact && surface < before.bottom())
		{
			// At the bottom of a valley, passive sliding cannot climb the opposite
			// ramp. Stop here instead of switching direction every frame.
			_vel.x = 0;
			_slopeContact = support;
			surface = before.bottom();
		}
		if (_slopeContact)
			_vel.y = (surface - before.bottom()) / dt;
	}

	_slopeRequestedY = _vel.y;
	_slopeTargetBottom = before.bottom() + _vel.y * dt;
}

bool DynamicObject::collidableWith(CollidableObject* obj)
{
	// ignore the side of a platform joining the top of the slope
	if (_slopeContact && obj->to<StaticObject*>() &&
		!obj->compenetrable() && !obj->to<Platform*>() && !obj->to<Slope*>())
	{
		const RectF ramp = _slopeContact->sceneCollider(), other = obj->sceneCollider();
		if (std::abs(other.top() - ramp.top()) < 0.001f &&
			std::abs(_slopeContact->surfaceY(other) - ramp.top()) < 0.001f &&
			(std::abs(other.left() - ramp.right()) < 0.001f ||
			 std::abs(other.right() - ramp.left()) < 0.001f))
			return false;
	}
	return CollidableObject::collidableWith(obj);
}


void DynamicObject::finishMovement(float dt)
{
	// Passive sliding is a displacement, not stored input velocity: neither
	// friction nor a blocked slide should create a velocity uphill.
	_vel.x = _slopeSliding ? 0 : _vel.x / _slopeSpeed;
	if (!_slopeContact)
		return;

	const float tolerance = 0.01f;
	// Collision callbacks can disable physics (death/transformation) or bounce us.
	if (!_collidable || _freezedPhysics || _slopeContact->killed() ||
		_vel.y < _slopeRequestedY - tolerance)
	{
		_slopeContact = nullptr;
		return;
	}
	bool ceiling = false;
	for (const auto& axis : _collisionAxes)
		ceiling = ceiling || axis == dir2vec(Direction::UP);
	const float bottom = sceneCollider().bottom();
	if (_slopeRequestedY < 0 && (ceiling || bottom > _slopeTargetBottom + 0.0001f))
	{
		// A ceiling blocks the climb: do not project the body through it.
		setPos(_movementStartPos);
		_vel = {0, 0};
		_slopeContact = nullptr;
		return;
	}
	if (_slopeRequestedY > 0 && bottom < _slopeTargetBottom - tolerance)
	{
		// A flat or moving platform above the ramp takes precedence.
		_slopeContact = nullptr;
		return;
	}

	if (_slopeContact->overlapsX(sceneCollider()))
	{
		// Recompute after a wall has shortened horizontal movement.
		setPos(pos() + PointF(0, _slopeContact->surfaceY(sceneCollider()) - sceneCollider().bottom()));
		_vel.y = 0;
		_collisions.push_back(_slopeContact);
		_collisionAxes.push_back(dir2vec(Direction::DOWN));
		_collisionDepths.push_back(0);
		_slopeContact->collision(this, true, Direction::UP);
		collision(_slopeContact, true, Direction::DOWN);
	}
	_slopeContact = nullptr;
}
