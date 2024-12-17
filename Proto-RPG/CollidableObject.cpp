// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "CollidableObject.h"
#include "Scene.h"
#include <list>
#include <algorithm>
#include "timeUtils.h"
#include "collisionUtils.h"
#include "sdlUtils.h"
#include "GameScene.h"

using namespace agp;

CollidableObject::CollidableObject(Scene* scene, const RotatedRectF& rrect, Sprite* sprite, int layer) :
	MovableObject(scene, rrect.toRect(), sprite, layer)
{
	_angle = rad2deg(rrect.angle);

	defaultCollider();

	// default collision: non compenetration
	_compenetrable = false;
	_collidable = true;
}


void CollidableObject::defaultCollider()
{
	// set collider to default (whole rect)
	_collider = RectF(0, 0, _rect.size.x, _rect.size.y);
	_collider.angle = deg2rad(_angle);
}

bool CollidableObject::intersectsRect(const RectF& r)
{
	if (!intersectsRectShallow(r))
		return false;

	Vec2Df axis;
	float depth;
	return checkCollisionSAT(sceneCollider().verticesVec(), r.verticesVec(), axis, depth);
}

bool CollidableObject::intersectsRectShallow(const RectF& r)
{
	return sceneCollider().boundingRect().intersects(r);
}

bool CollidableObject::intersectsLine(const LineF& line, float& tNear)
{
	return sceneCollider().intersectsLine(line.start, line.end, tNear);
}

void CollidableObject::update(float dt)
{
	MovableObject::update(dt);

	detectCollisions();
	resolveCollisions();
}

RotatedRectF CollidableObject::sceneCollider() const
{
	return _collider + _rect.pos;
}

void CollidableObject::detectCollisions()
{
	if (!_collidable)
		return;

	_collisionsPrev = _collisions;
	_collisions.clear();
	_collisionAxes.clear();
	_collisionDepths.clear();

	auto objectsInRect = _scene->objects(sceneCollider().boundingRect());
	for (auto& obj : objectsInRect)
	{
		CollidableObject* collObj = obj->to<CollidableObject*>();
		if (collObj && collObj != this && collObj->collidable() && collidableWith(collObj))
		{
			Vec2Df axis;
			float depth;
			if (checkCollisionSAT(sceneCollider().verticesVec(), collObj->sceneCollider().verticesVec(), axis, depth))
			{
				_collisions.push_back(collObj);
				_collisionAxes.push_back(axis);
				_collisionDepths.push_back(depth);
				collision(collObj, true, axis);
				collObj->collision(this, true, -axis);
			}
		}
	}

	// first remove objects marked 'to be killed' from collision list
	// since they will not be accessible in the next iteration
	size_t j = 0;
	for (size_t i = 0; i < _collisions.size(); ++i)
	{
		if (!_collisions[i]->_killed)
		{
			_collisions[j] = _collisions[i];
			_collisionAxes[j] = _collisionAxes[i];
			_collisionDepths[j] = _collisionDepths[i];
			j++;
		}
	}
	_collisions.resize(j);
	_collisionAxes.resize(j);
	_collisionDepths.resize(j);

	// decollisions = previous collisions that are no more
	for(auto collObj : _collisionsPrev)
		if (std::find(_collisions.begin(), _collisions.end(), collObj) == _collisions.end())
		{
			collision(collObj, false, Vec2Df());
			collObj->collision(this, false, Vec2Df());
		}
}

bool CollidableObject::collision(CollidableObject* with, bool begin, const Vec2Df& normal)
{
	//printf("%s collided with %s from %s\n", name().c_str(), with->name().c_str(), normal.str().c_str());
	return true; 
}

void CollidableObject::draw(SDL_Renderer* renderer, Transform camera)
{
	MovableObject::draw(renderer, camera);

	GameScene* gameScene = dynamic_cast<GameScene*>(_scene);
	if (gameScene && gameScene->collidersVisible())
	{
		std::array < PointF, 4> drawVertices = sceneCollider().vertices();
		for (int i = 0; i < 4; i++)
			drawVertices[i] = camera(drawVertices[i]);
		
		DrawOBB(renderer, drawVertices, _colliderColor);
	}
}

float CollidableObject::distance(CollidableObject* obj) const
{
	return sceneCollider().center.distance(obj->sceneCollider().center);
}