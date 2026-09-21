// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "RenderableObject.h"
#include "box2d/box2d.h"

namespace agp
{
	class GameScene;
	class RigidObject;

	// collision category masks (max 32)
	enum class CollisionCategory
	{
		Static		= 1 << 0,		// 1 shifted left by 0 bits: 0001
		Dynamic		= 1 << 1,		// 1 shifted left by 1 bit:  0010
		Kinematic	= 1 << 2,		// ...
		Player		= 1 << 3,
		Weapon		= 1 << 4,
		Enemy		= 1 << 5
	};
}

// RigidObject class.
// - uses Box2D's physics engine
// - provides logic collision interface (based on Box2D v3.*)
// - adds oriented bounding box as reference bounding shape used for rendering and default physics
class agp::RigidObject : public RenderableObject
{
	protected:

		b2BodyId _bodyId;
		RotatedRectF _obb;		// oriented bounding box
		const Color _bodyFrameColor = { 255, 0, 0, 255 };
		const Color _colliderColor = { 255, 255, 0, 255 };

	public:

		RigidObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, b2BodyType type, int layer = 0);
		virtual ~RigidObject() { if(b2Body_IsValid(_bodyId)) b2DestroyBody(_bodyId); }

		// getters / setters
		b2Vec2 vel() const { return b2Body_GetLinearVelocity(_bodyId); }
		b2BodyId bodyId() const { return _bodyId; }
		
		// extends game logic (+physics)
		virtual void update(float dt) override;

		// extends rendering (+collider)
		virtual void draw(SDL_Renderer* renderer, Transform camera) override;

		// extends freezed state (+Box2D disable)
		virtual void setFreezed(bool on) override;

		// defines logic collision, i.e. what to do when two objects collide
		// WARNING: Box2D does not define normal when collision ends (begin=false)
		virtual void collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB);

		virtual std::string name() override {
			return strprintf("RigidObject[%d]", _id);
		}
};