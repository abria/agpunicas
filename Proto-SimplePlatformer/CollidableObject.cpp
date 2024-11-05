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

using namespace agp;

CollidableObject::CollidableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	MovableObject(scene, rect, sprite, layer)
{
	// default collider: object rect
	_collider = { 0, 0, _rect.size.x, _rect.size.y };

	// default collision: non compenetration
	_compenetrable = false;
	_collidable = true;
}

void CollidableObject::defaultCollider()
{
	_collider = { 0, 0, _rect.size.x, _rect.size.y };
}

void CollidableObject::update(float dt)
{
	MovableObject::update(dt);

	// undo move since collision detection is based on CCD
	_rect.pos -= _vel * dt;
	
	// detect and resolve collisions by updating velocities
	resolveCollisions(dt);

	// move with updated velocity
	_rect.pos += _vel * dt;
}

RectF CollidableObject::sceneCollider() const
{
	return _collider + _rect.pos;
}

void CollidableObject::resolveCollisions(float dt)
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
	for (auto obj : sortedByContactTime)
		if (DynamicRectVsRect(sceneCollider(), vel() * dt, obj.first->sceneCollider(), cp, cn, ct))
		{
			if (!obj.first->compenetrable())
				velAdd(-cn * cn.dot(_vel * (1 - ct)));

			obj.first->collision(this, normal2dir(cn));
			collision(obj.first, inverse(normal2dir(cn)));
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