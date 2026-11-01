// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DynamicObject.h"

using namespace agp;

DynamicObject::DynamicObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	CollidableObject(scene, rect, sprite, layer)
{
	// Dynamic objects handle contacts with each other through game logic.
	_compenetrable = true;
	_facingDir = Direction::RIGHT;
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
	_slopeMotion = Slope::prepareMovement(*this, _vel, _xDir == Direction::NONE, _xVelMin, dt);
}

void DynamicObject::finishMovement(float dt)
{
	if (!_collidable || _freezedPhysics)
		_slopeMotion.support = nullptr;
	bool ceiling = false;
	for (const auto& contact : _contacts)
		ceiling = ceiling || (!contact.first->compenetrable() && contact.second == Direction::UP);

	// Let the slope finalize the collider after walls, ceilings and platforms.
	RectF body = sceneCollider();
	Slope* support = _slopeMotion.finishMovement(body, _vel, movementStartCollider(), ceiling);
	if (body.pos != sceneCollider().pos)
		setPos(pos() + (body.pos - sceneCollider().pos));
	if (support)
		_contacts[support] = Direction::DOWN;
	_slopeMotion.support = nullptr; // recomputed next step; recorded contacts remain
}

bool DynamicObject::collidableWith(CollidableObject* obj)
{
	if (_slopeMotion.support && _slopeMotion.support->joinsTop(obj))
		return false;
	return CollidableObject::collidableWith(obj);
}
