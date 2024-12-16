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
#include "GameScene.h"
#include "DynamicObject.h"
#include "StaticObject.h"

using namespace agp;

CollidableObject::CollidableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	MovableObject(scene, rect, sprite, layer)
{
	// default collider: object rect
	_collider = { 0, 0, _rect.size.x, _rect.size.y };

	// default collision: non compenetration
	_compenetrable = false;
	_collidable = true;

	// default collision system: Continous Collision Detection (CCD)
	_CCD = false;
}

void CollidableObject::defaultCollider()
{
	_collider = { 0, 0, _rect.size.x, _rect.size.y };
}

void CollidableObject::update(float dt)
{
	MovableObject::update(dt);

	if (_CCD)
	{
		// undo move since collision detection is based on CCD
		_rect.pos -= _vel * dt;

		// detect and resolve collisions by updating velocities
		detectResolveCollisionsCCD(dt);

		// move with updated velocity
		_rect.pos += _vel * dt;
	}
	else
	{
		detectCollisions();
		resolveCollisions();
	}
}

RectF CollidableObject::sceneCollider() const
{
	return _collider + _rect.pos;
}

void CollidableObject::detectResolveCollisionsCCD(float dt)
{
	if (!_collidable)
		return;

	// NARROW collision detection
	// simulate next iteration pos to get objects within united bounding rect
	PointF curPos = _rect.pos;
	RectF curRect = sceneCollider();
	_rect.pos += _vel * dt;
	std::list<CollidableObject*> likely_collisions;
	std::list<Object*> items_in_rect = _scene->objects(sceneCollider().united(curRect));
	for (auto item : items_in_rect)
	{
		CollidableObject* obj = item->to<CollidableObject*>();
		if (obj && obj != this && obj->collidable() && collidableWith(obj))
			likely_collisions.push_back(obj);
	}
	_rect.pos = curPos;	// restore current pos

	// sort collisions in ascending order of contact time
	Vec2Df cp, cn;
	float ct = 0, min_t = INFINITY;
	std::vector<std::pair<CollidableObject*, float>> sortedByContactTime;
	for (auto& obj : likely_collisions)
		if (DynamicRectVsRect(sceneCollider(), vel() * dt, obj->sceneCollider(), cp, cn, ct))
			sortedByContactTime.push_back({ obj, ct });
	std::sort(sortedByContactTime.begin(), sortedByContactTime.end(),
		[this](const std::pair<CollidableObject*, float>& a, const std::pair<CollidableObject*, float>& b)
		{
			// if contact time is the same, give priority to nearest object
			return a.second != b.second ? a.second < b.second : distance(a.first) < distance(b.first);
		});

	// solve the collisions in correct order
	_collisionsPrev = _collisions;
	_collisions.clear();
	for (auto obj : sortedByContactTime)
		if (DynamicRectVsRect(sceneCollider(), vel() * dt, obj.first->sceneCollider(), cp, cn, ct))
		{
			if (!obj.first->compenetrable())
			{
				velAdd(-cn * cn.dot(_vel * (1 - ct)));
				_collisions.push_back(obj.first);
			}
			else
				_collisionsCompenetrables.insert(obj.first);

			obj.first->collision(this, true, normal2dir(cn));
			collision(obj.first, true, inverse(normal2dir(cn)));
		}

	// detect de-collisions with compenetrables
	for (auto it = _collisionsCompenetrables.begin(); it != _collisionsCompenetrables.end(); )
	{
		// remove objects marked 'to be killed' from collision list
		// since they will not be accessible in the next iteration
		if ((*it)->_killed)
			it = _collisionsCompenetrables.erase(it);
		else if (sceneCollider().isSeparatedFrom((*it)->sceneCollider(), 0.1f))
		{
			collision(*it, false, Direction::NONE);
			(*it)->collision(this, false, Direction::NONE);

			it = _collisionsCompenetrables.erase(it);
		}
		else
			++it;
	}

	// detect de-collisions with compenetrables
	// first remove objects marked 'to be killed' from collision list
	// since they will not be accessible in the next iteration
	_collisions.erase(std::remove_if(_collisions.begin(), _collisions.end(), [](CollidableObject* obj) { return obj->_killed; }), _collisions.end());
	for (auto collObj : _collisionsPrev)
		if (std::find(_collisions.begin(), _collisions.end(), collObj) == _collisions.end())
		{
			collision(collObj, false, Direction::NONE);
			collObj->collision(this, false, Direction::NONE);
		}
}

void CollidableObject::detectCollisions()
{
	if (!_collidable)
		return;

	_collisionsPrev = _collisions;
	_collisions.clear();
	_collisionAxes.clear();
	_collisionDepths.clear();

	auto objectsInRect = _scene->objects(sceneCollider());
	for (auto& obj : objectsInRect)
	{
		CollidableObject* collObj = obj->to<CollidableObject*>();
		if (collObj && collObj != this && collObj->collidable() && collidableWith(collObj))
		{
			Direction axis;
			float depth;
			if (checkCollisionAABB(sceneCollider(), collObj->sceneCollider(), axis, depth))
			{
				_collisions.push_back(collObj);
				_collisionAxes.push_back(dir2vec(axis));
				_collisionDepths.push_back(depth);
				collision(collObj, true, axis);
				collObj->collision(this, true, inverse(axis));
			}
		}
	}

	// remove objects marked 'to be killed' from collision list
	// since they will not be accessible in the next iteration
	_collisions.erase(std::remove_if(_collisions.begin(), _collisions.end(), [](CollidableObject* obj) { return obj->_killed; }), _collisions.end());

	for (auto collObj : _collisionsPrev)
		if (std::find(_collisions.begin(), _collisions.end(), collObj) == _collisions.end())
		{
			collision(collObj, false, Direction::NONE);
			collObj->collision(this, false, Direction::NONE);
		}
}

void CollidableObject::resolveCollisions()
{
	for (int i = 0; i < _collisions.size(); i++)
	{
		DynamicObject* dynObj = _collisions[i]->to<DynamicObject*>();
		StaticObject* staticObj = _collisions[i]->to<StaticObject*>();

		// Dynamic vs. Static: hard non-compenetration constraint
		if (staticObj)
			_rect.pos += -_collisionAxes[i] * _collisionDepths[i];
		// Dynamic vs. Dynamic: soft non-compenetration constraint
		else if (dynObj)
			_rect.pos += -_collisionAxes[i] * _collisionDepths[i] / 10.0f;
	}
}

void CollidableObject::draw(SDL_Renderer* renderer, Transform camera)
{
	MovableObject::draw(renderer, camera);

	GameScene* gameScene = dynamic_cast<GameScene*>(_scene);
	if (gameScene && gameScene->collidersVisible())
	{
		auto vertices = sceneCollider().vertices();
		SDL_FRect drawRect = RectF(camera(vertices[0]), camera(vertices[2])).toSDLf();
		SDL_SetRenderDrawColor(renderer, _colliderColor.r, _colliderColor.g, _colliderColor.b, _colliderColor.a);
		SDL_RenderDrawRectF(renderer, &drawRect);
	}
}

float CollidableObject::distance(CollidableObject* obj) const
{
	return sceneCollider().center().distance(obj->sceneCollider().center());
}