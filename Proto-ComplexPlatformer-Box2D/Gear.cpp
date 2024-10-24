// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Gear.h"
#include "mathUtils.h"

using namespace agp;

Gear::Gear(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer) :
	KinematicObject(scene, obb, sprite, layer)
{
	b2Circle circle;
	circle.center = { 0,0 };
	float s = 0.9f;
	circle.radius = (obb.size.x / 2) * s;
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1;
	shapeDef.friction = 0.2f;
	shapeDef.restitution = 0.0;
	b2CreateCircleShape(_bodyId, &shapeDef, &circle);

	float angle_step = 2 * PI / 12;
	for (int i = 0; i < 12; i++)
	{
		RotatedRectF teeth({ 0, _obb.size.y * s / 2 }, { _obb.size.x * 0.11f, _obb.size.y * 0.05f });
		teeth.center.y += teeth.size.y / 2;
		
		teeth.center = teeth.center.rot(i * angle_step, { 0,0 }, true);
		teeth.angle = i * angle_step;
		shapeDef.density = 1;
		shapeDef.friction = 0.2f;
		shapeDef.restitution = 0.0;
		b2Polygon boxDef = b2MakeOffsetBox(teeth.size.x/2, teeth.size.y/2, teeth.center.toB2(), b2MakeRot(teeth.angle));
		b2CreatePolygonShape(_bodyId, &shapeDef, &boxDef);
	}

	b2Body_SetAngularVelocity(_bodyId, -0.5f);
}