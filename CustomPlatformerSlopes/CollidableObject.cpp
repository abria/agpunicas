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
	//if (_CCD)
	//	return MovableObject::falling();

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
	if (dt <= 0)
		return;
	MovableObject::update(dt);
	if (_freezedPhysics)
		return;

	_fallingPrev = falling();
	// Return to the start of the step before choosing the supporting surface.
	setPos(pos() - _vel * dt);
	prepareMovement(dt);
	if (_CCD)
	{
		detectResolveCollisionsCCD(dt);
		setPos(pos() + _vel * dt);
	}
	else
	{
		setPos(pos() + _vel * dt);
		detectCollisionsAABB();
		resolveCollisionsAABB();
	}
	finishMovement(dt);
	// Slope contacts must be recorded before checking for ended contacts.
	detectDecollisions();
}

bool CollidableObject::sweep(const RectF& body, const Vec2Df& movement,
	Vec2Df& point, Vec2Df& normal, float& time) const
{
	return DynamicRectVsRect(body, movement, sceneCollider(), point, normal, time);
}

bool CollidableObject::overlap(const RectF& body, Direction& axis, float& depth) const
{
	return checkCollisionAABB(body, sceneCollider(), axis, depth);
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
		if (obj && obj != this && !obj->killed() && obj->collidable() && collidableWith(obj) && obj->collidableWith(this))
			likely_collisions.push_back(obj);
	}
	setPos(curPos);	// restore current pos

	// sort collisions in ascending order of contact time
	Vec2Df cp, cn;
	float ct = 0;
	std::vector<std::pair<CollidableObject*, float>> sortedByContactTime;
	for (auto& obj : likely_collisions)
		if (obj->sweep(sceneCollider(), vel() * dt, cp, cn, ct) && collidableWith(obj) && obj->collidableWith(this))
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
		if (obj.first->sweep(sceneCollider(), vel() * dt, cp, cn, ct))
		{
			if (!obj.first->compenetrable())
			{
				// Resolve the current displacement without clamping slope-scaled speed.
				_vel -= cn * cn.dot(_vel * (1 - std::max(0.0f, std::min(ct, 1.0f))));
				if (std::abs(_vel.y) < _yVelMin) _vel.y = 0;
				_collisions.push_back(obj.first);
				_collisionAxes.push_back(-cn);
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
		if (collObj && collObj != this && !collObj->killed() && collObj->collidable() && collidableWith(collObj) && collObj->collidableWith(this))
		{
			Direction axis;
			float depth;
			if (collObj->overlap(sceneCollider(), axis, depth))
			{
				_collisions.push_back(collObj);
				_collisionAxes.push_back(dir2vec(axis));
				_collisionDepths.push_back(depth);
				collision(collObj, true, axis);
				collObj->collision(this, true, inverse(axis));
			}
		}
	}

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
		SDL_RenderRect(renderer, &drawRect);
	}
}

float CollidableObject::distance(CollidableObject* obj) const
{
	return sceneCollider().center().distance(obj->sceneCollider().center());
}