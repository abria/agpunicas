// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Fire.h"
#include "SpriteFactory.h"

using namespace agp;

Fire::Fire(GameScene* scene, const PointF& spawnPoint, const Vec2Df& velocity)
	: DynamicObject(scene, RotatedRectF(spawnPoint, {1,1}, 0, true), SpriteFactory::instance()->get("fire"), 1)
{
	_xVelMax = 10000;
	_yVelMax = 10000;

	// default box shape
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1;
	shapeDef.friction = 0.2f;
	shapeDef.restitution = 0;
	b2Polygon boxDef = b2MakeOffsetRoundedBox(_obb.size.x / 4, _obb.size.y / 4, { 0, 0 }, b2MakeRot(_obb.angle), 0.2f);
	b2CreatePolygonShape(_bodyId, &shapeDef, &boxDef);

	b2Body_SetFixedRotation(_bodyId, true);
	b2Body_SetGravityScale(_bodyId, 0);
	b2Body_SetLinearVelocity(_bodyId, velocity.toB2());
}

void Fire::update(float dt)
{
	DynamicObject::update(dt);

	_flip = vel().x < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
}