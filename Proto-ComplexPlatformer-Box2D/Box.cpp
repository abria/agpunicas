// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Box.h"
#include "SpriteFactory.h"

using namespace agp;

Box::Box(GameScene* scene, const RotatedRectF& obb)
	: DynamicObject(scene, obb, SpriteFactory::instance()->get("box"))
{
	//_xVelMax = 10000;
	//_yVelMax = 10000;

	// default box shape
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1;
	shapeDef.friction = 0.2f;
	shapeDef.restitution = 0;
	shapeDef.enableContactEvents = true;
	b2Polygon boxDef = b2MakeOffsetBox(_obb.size.x / 2, _obb.size.y / 2, { 0, 0 }, b2MakeRot(_obb.angle));
	b2CreatePolygonShape(_bodyId, &shapeDef, &boxDef);

	b2Body_ApplyLinearImpulseToCenter(_bodyId, { (rand()%10)-5.0f, 10}, true);
}