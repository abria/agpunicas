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
#include "KinematicObject.h"

using namespace agp;

CollidableObject::CollidableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	MovableObject(scene, rect, sprite, layer)
{
	defaultCollider();

	// default collision: non compenetration
	_compenetrable = false;
	_collidable = true;

	// default collision system: Continous Collision Detection (CCD)
	_CCD = true;

	_fallingPrev = false;
}

void CollidableObject::defaultCollider()
{
	_collider = { 0, 0, rect().size.x, rect().size.y};
}

void CollidableObject::setCCD(bool active)
{
	_CCD = active;
}

bool CollidableObject::grounded() const
{
	if (_CCD)
		return MovableObject::grounded();

	return _fallingPrev && !falling();
}

bool CollidableObject::falling() const
{
	if (_CCD)
		return MovableObject::falling();

	return midair() && _vel.y > 0;
}

bool CollidableObject::midair() const
{
	if (_CCD)
		return MovableObject::midair();

	// check for collisions with Static or Kinematic below current object
	for (int k = 0; k < _collisions.size(); k++)
		if ((_collisions[k]->to<StaticObject*>() || _collisions[k]->to<KinematicObject*>())
			&& _collisionAxes[k] == dir2vec(Direction::DOWN))
			return false;

	return true;
}

void CollidableObject::update(float dt)
{
	MovableObject::update(dt);

	_fallingPrev = falling();

	if (_CCD)
	{
		// undo move since collision detection is based on CCD
		setPos(pos() - _vel * dt);

		// detect and resolve collisions by updating velocities
		detectResolveCollisionsCCD(dt);

		// move with updated velocity
		setPos(pos() + _vel * dt);
	}
	else
	{
		detectCollisionsAABB();
		resolveCollisionsAABB();
	}
}

RectF CollidableObject::sceneCollider() const
{
	return _collider + rect().pos;
}

void CollidableObject::detectDecollisions()
{
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
	for (auto collObj : _collisionsPrev)
		if (std::find(_collisions.begin(), _collisions.end(), collObj) == _collisions.end())
		{
			collision(collObj, false, Direction::NONE);
			collObj->collision(this, false, Direction::NONE);
		}
}

void CollidableObject::detectResolveCollisionsCCD(float dt)
{
	if (!_collidable)
		return;

	// NARROW collision detection
	// simulate next iteration pos to get objects within united bounding rect
	PointF curPos = pos();
	RectF curRect = sceneCollider();
	setPos(pos() + _vel * dt);
	std::vector<CollidableObject*> likely_collisions;
	Objects items_in_rect = _scene->objects(sceneCollider().united(curRect));
	for (auto item : items_in_rect)
	{
		CollidableObject* obj = item->to<CollidableObject*>();
		if (obj && obj != this && obj->collidable() && collidableWith(obj))
			likely_collisions.push_back(obj);
	}
	setPos(curPos);	// restore current pos

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
	// also update collision metadata
	_collisionsPrev = _collisions;
	_collisions.clear();
	_collisionAxes.clear();
	_collisionDepths.clear();
	for (auto& obj : sortedByContactTime)
		if (DynamicRectVsRect(sceneCollider(), vel() * dt, obj.first->sceneCollider(), cp, cn, ct))
		{
			if (!obj.first->compenetrable())
			{
				velAdd(-cn * cn.dot(_vel * (1 - ct)));
				_collisions.push_back(obj.first);
				_collisionAxes.push_back(cn);
				_collisionDepths.push_back(0);
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

	// detect de-collisions with noncompenetrables
	detectDecollisions();
}

void CollidableObject::detectCollisionsAABB()
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

	detectDecollisions();
}

void CollidableObject::resolveCollisionsAABB()
{
	for (int i = 0; i < _collisions.size(); i++)
	{
		DynamicObject* dynObj = _collisions[i]->to<DynamicObject*>();
		StaticObject* staticObj = _collisions[i]->to<StaticObject*>();
		KinematicObject* kinObj = _collisions[i]->to<KinematicObject*>();

		// Dynamic vs. Static or Kinematic: hard non-compenetration constraint
		if (staticObj || kinObj)
			setPos(pos() -_collisionAxes[i] * _collisionDepths[i]);
		// Dynamic vs. Dynamic: soft non-compenetration constraint
		else if (dynObj)
			setPos(pos() -_collisionAxes[i] * _collisionDepths[i] / 10.0f);
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