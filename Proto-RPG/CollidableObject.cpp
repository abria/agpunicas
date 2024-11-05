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
	defaultCollider();

	// default collision: non compenetration
	_compenetrable = false;
	_collidable = true;
}

// set collider to default (whole rect)
void CollidableObject::defaultCollider()
{
	_collider = { 0, 0, _rect.size.x, _rect.size.y };
}

void CollidableObject::update(float dt)
{
	MovableObject::update(dt);

	detectCollisions();
	resolveCollisions();
}

RectF CollidableObject::sceneCollider() const
{
	return _collider + _rect.pos;
}

void CollidableObject::detectCollisions()
{
	if (!_collidable)
		return;

	_collisions.clear();
	_collisionAxes.clear();
	_collisionDepths.clear();

	auto objectsInRect = _scene->objects(_rect);
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
				collision(collObj, axis);
				collObj->collision(this, inverse(axis));
			}
		}
	}
}

bool CollidableObject::collision(CollidableObject* with, Direction fromDir) 
{
	//printf("%s collided with %s from %s\n", name().c_str(), with->name().c_str(), dir2str(fromDir).c_str());
	return true; 
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