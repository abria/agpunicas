// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "MovableObject.h"
#include <vector>

namespace agp
{
	class CollidableObject;
}

// CollidableObject class.
// - defines a collider (default: object rect)
// - implements collision detection and resolution
// - defines logic collision (see collision method)
class agp::CollidableObject : public MovableObject
{
	protected:

		// collisions
		RotatedRectF _collider;					// in object coordinates
		bool _collidable;
		bool _compenetrable;
		const Color _colliderColor = { 255, 255, 0, 255 };
		std::vector<CollidableObject*> _collisions;
		std::vector<Vec2Df> _collisionAxes;
		std::vector<float> _collisionDepths;
		std::vector<CollidableObject*> _collisionsPrev;

		// collision detection/resolution
		virtual void detectCollisions();
		virtual void resolveCollisions() = 0;  // see Dynamic and Static objects

		// set collider to default (whole rect)
		void defaultCollider();

	public:

		CollidableObject(Scene* scene, const RotatedRectF& rrect, Sprite* sprite, int layer = 0);
		virtual ~CollidableObject() {}

		// getters/setters
		const RotatedRectF& collider() const { return _collider; }
		RotatedRectF sceneCollider() const;
		bool compenetrable() const { return _compenetrable; }
		bool collidable() const { return _collidable; }

		// extends intersection (+rotated rect)
		virtual bool intersectsRectShallow(const RectF& r) override;

		// extends line intersection (+rotated rect)
		virtual bool intersectsLine(const LineF& line, float& tNear) override;

		// extends game logic (+collisions)
		virtual void update(float dt) override;

		// extends rendering (+collider)
		virtual void draw(SDL_Renderer* renderer, Transform camera) override;

		// defines acceptable collisions (default: any)
		virtual bool collidableWith(CollidableObject* obj) { return true; }

		// defines logic collision, i.e. what to do when two objects collide
		// from a game logic perspective (e.g. player dies if hit by enemy)
		// returns true if logic collision is resolved, false otherwise
		virtual bool collision(CollidableObject* with, bool begin, const Vec2Df& normal);

		// euclidean distance between colliders
		virtual float distance(CollidableObject* obj) const;
};