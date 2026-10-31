// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "RigidObject.h"
#include "PlatformerGameScene.h"
#include "sdlUtils.h"
#include "View.h"

using namespace agp;

RigidObject::RigidObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, b2BodyType type, int layer) :
	RenderableObject(scene, obb.toRect(), sprite, layer)
{
	_obb = obb;

	// default body
	// note: shapes need to be defined in subclasses
	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.position = { obb.center.x, obb.center.y };
	bodyDef.rotation = b2MakeRot(_obb.angle);
	bodyDef.motionLocks.angularZ = false;
	bodyDef.userData = this;
	bodyDef.type = type;
	_bodyId = b2CreateBody(dynamic_cast<PlatformerGameScene*>(scene)->worldId(), &bodyDef);
}

void RigidObject::update(float dt)
{
	RenderableObject::update(dt);

	// update position and orientation
	b2Vec2 pos = b2Body_GetPosition(_bodyId);
	_obb.center = { pos.x, pos.y };
	_obb.angle = b2Rot_GetAngle(b2Body_GetRotation(_bodyId));
	setRect(_obb.toRect());
	_angle = rad2deg(_obb.angle);
}

void RigidObject::draw(SDL_Renderer* renderer, Transform camera)
{
	RenderableObject::draw(renderer, camera);

	GameScene* gameScene = dynamic_cast<GameScene*>(_scene);
	if (gameScene && gameScene->collidersVisible())
	{
		// draw body reference frame
		RotatedRectF body_obb(camera(_obb.center), _scene->view()->magf() * _obb.size, _obb.angle);
		auto body_vertices = body_obb.vertices();
		DrawThickPolygon(renderer, body_vertices.data(), 4, _colliderPixelThickness, _bodyFrameColor);

		// draw shapes
		int shapes_count = b2Body_GetShapeCount(_bodyId);
		b2ShapeId shapes_ids[20];
		b2Body_GetShapes(_bodyId, shapes_ids, 20);
		for (int i = 0; i < shapes_count; i++)
		{
			b2ShapeType shape_type = b2Shape_GetType(shapes_ids[i]);
			if (shape_type == b2ShapeType::b2_polygonShape)
			{
				b2Polygon poly = b2Shape_GetPolygon(shapes_ids[i]);
				PointF poly_vertices[B2_MAX_POLYGON_VERTICES];
				for (int k = 0; k < poly.count; k++)
				{
					b2Vec2 vw = b2Body_GetWorldPoint(_bodyId, poly.vertices[k]);
					poly_vertices[k] = camera(Vec2Df(vw.x, vw.y));
				}
				DrawThickPolygon(renderer, poly_vertices, poly.count, _colliderPixelThickness, _colliderColor);
			}
			else if (shape_type == b2ShapeType::b2_capsuleShape)
			{
				b2Capsule capsule = b2Shape_GetCapsule(shapes_ids[i]);
				b2Vec2 c1w = b2Body_GetWorldPoint(_bodyId, capsule.center1);
				b2Vec2 c2w = b2Body_GetWorldPoint(_bodyId, capsule.center2);
				PointF c1 = camera(Vec2Df(c1w.x, c1w.y));
				PointF c2 = camera(Vec2Df(c2w.x, c2w.y));
				float radius = capsule.radius * _scene->view()->magf().x;

				DrawThickCapsule(renderer, c2, c1, radius, _colliderPixelThickness, _colliderColor, 100);
			}
			else if (shape_type == b2ShapeType::b2_circleShape)
			{
				b2Circle circle = b2Shape_GetCircle(shapes_ids[i]);
				b2Vec2 c = b2Body_GetWorldPoint(_bodyId, circle.center);
				PointF center = camera(Vec2Df(c.x, c.y));
				float radius = circle.radius * _scene->view()->magf().x;

				DrawThickCircle(renderer, center, radius, _colliderPixelThickness, _colliderColor, 100);
			}
		}
	}
}

void RigidObject::collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB)
{
	/*printf("%s %s collision with %s from %s\n", 
		this->name().c_str(), 
		(begin ? "BEGINS" : "ENDS"), with->name().c_str(), normal.str().c_str());*/
}

void RigidObject::setFreezed(bool on) 
{ 
	if (on != _freezed)
	{
		if (on)
			b2Body_Disable(_bodyId);
		else
			b2Body_Enable(_bodyId);
	}
	Object::setFreezed(on);
}