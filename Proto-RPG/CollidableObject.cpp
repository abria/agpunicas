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