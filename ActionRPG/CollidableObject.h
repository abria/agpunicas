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
#include <map>

namespace agp
{
	class CollidableObject;
	enum class Contact { BEGIN, STAY, END };
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
		const float _colliderPixelThickness = 3;	// thickness of collider lines (C), in screen pixels
		std::map<CollidableObject*, Vec2Df> _contacts, _contactsPrev;
		const float _contactTolerance = 0.01f;
		bool _detectsContacts = true; // false for static objects and triggers

		// collision detection/resolution
		void detectContacts();
		void notifyContact(CollidableObject* with, Contact contact, const Vec2Df& normal);
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
		virtual bool intersectsRect(const RectF& r) override;
		virtual bool intersectsRectShallow(const RectF& r) override;

		// extends line intersection (+rotated rect)
		virtual bool intersectsLine(const LineF& line, float& tNear) override;

		// extends game logic (+collisions)
		virtual void update(float dt) override;
		virtual void forgetObject(Object* objectToRemove) override;

		// extends rendering (+collider)
		virtual void draw(SDL_Renderer* renderer, Transform camera) override;

		// defines acceptable collisions (default: any)
		virtual bool collidableWith(CollidableObject* obj) { return true; }

		// defines logic collision, i.e. what to do when two objects collide
		// from a game logic perspective (e.g. player dies if hit by enemy)
		// BEGIN/END notify both objects once; STAY runs during each active object
		// update and is forwarded to passive objects. END has a zero normal.
		// Returns true if the interaction is handled.
		virtual bool collision(CollidableObject* with, Contact contact, const Vec2Df& normal);

		// euclidean distance between colliders
		virtual float distance(CollidableObject* obj) const;
};