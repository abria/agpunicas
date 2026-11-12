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
	private:

		RectF _movementStartCollider; // scene coordinates, saved before movement hooks

	protected:

		// collisions
		RectF _collider;			// in object coordinates
		bool _collidable;
		bool _compenetrable;
		const Color _colliderColor = { 255, 255, 0, 255 };
		const float _colliderPixelThickness = 3;	// thickness of collider lines (C), in screen pixels
		bool _CCD;
		std::map<CollidableObject*, Direction> _contacts, _contactsPrev;
		const float _contactTolerance = 0.01f; // touching counts as contact, in scene units
		bool _detectsContacts = true; // false when contacts are reported by touching objects
		bool _fallingPrev;

		// movement hooks shared by CCD and discrete collision resolution
		// prepare: constrain the planned move; finish: finalize state and contacts
		virtual void prepareMovement(float dt) {}
		virtual void finishMovement(float dt) {}

		// CCD collision detection/resolution
		virtual void resolveCollisionsCCD(float dt);

		// AABB intersection-based collision detection and resolution
		virtual void resolveCollisionsAABB();

		// compare current contacts with the previous update
		void detectContacts();
		void notifyContact(CollidableObject* with, Contact contact, Direction fromDir);

		// set collider to default (whole rect)
		void defaultCollider();

	public:

		CollidableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);
		virtual ~CollidableObject() {}

		// getters/setters
		const RectF& collider() const { return _collider; }
		RectF sceneCollider() const;
		const RectF& movementStartCollider() const { return _movementStartCollider; }
		bool compenetrable() const { return _compenetrable; }
		bool collidable() const { return _collidable; }
		void setCCD(bool active);

		// Shape-specific collision tests (AABB by default).
		virtual bool sweep(const RectF& body, const Vec2Df& movement,
			Vec2Df& point, Vec2Df& normal, float& time) const;
		virtual bool overlap(const RectF& body, Direction& axis, float& depth) const;

		// Reuse shape-specific overlap tests; tolerance bridges faces, not corners.
		bool touches(const RectF& body, float tolerance, Direction& side) const;

		// extends state queries (+CCD off)
		virtual bool grounded() const override;
		virtual bool falling() const override;
		virtual bool midair() const override;

		// extends game logic (+collisions)
		virtual void update(float dt) override;
		virtual void forgetObject(Object* objectToRemove) override;

		// extends rendering (+collider)
		virtual void draw(SDL_Renderer* renderer, Transform camera) override;

		// defines acceptable collisions (default: any)
		virtual bool collidableWith(CollidableObject* obj) { return true; }

		// defines logic collision, i.e. what to do when two objects collide
		// from a game logic perspective (e.g. player dies if hit by enemy)
		// BEGIN once on entry, STAY while touching, END once on exit.
		// A trigger crossed within one CCD step also counts (END at the next check).
		// BEGIN/END notify both objects; STAY runs on each active object
		// during its own update, and is forwarded to passive objects.
		// fromDir is the side touched (NONE for END); returns whether handled.
		virtual bool collision(CollidableObject* with, Contact contact, Direction fromDir) { return true; }

		// euclidean distance between colliders
		virtual float distance(CollidableObject* obj) const;
};
