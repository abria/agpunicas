// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "StaticObject.h"
#include "Scene.h"

using namespace agp;

StaticObject::StaticObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer) :
	RigidObject(scene, obb, sprite, b2BodyType::b2_staticBody, layer)
{	
	// default box shape
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1;
	shapeDef.friction = 0.2f;
	shapeDef.restitution = 0.0;
	b2Polygon boxDef = b2MakeOffsetBox(_obb.size.x / 2, _obb.size.y / 2, { 0, 0 }, {1, 0 });
	b2CreatePolygonShape(_bodyId, &shapeDef, &boxDef);
}

void StaticObject::collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB)
{
	RigidObject::collision(with, begin, normal, shapeA, shapeB);
}
