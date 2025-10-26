// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DynamicObject.h"
#include "StaticObject.h"
#include "KinematicObject.h"

using namespace agp;

DynamicObject::DynamicObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer) :
	RigidObject(scene, obb, sprite, b2BodyType::b2_dynamicBody, layer)
{
	_xDir = Direction::RIGHT;
	_xMoveForce = 20;
	_yJumpImpulse = 30;
	_xVelMax = 1;
	_yVelMax = 5;
}

void DynamicObject::update(float dt)
{
	RigidObject::update(dt);

	//// clip velocity
	//b2Vec2 vel = b2Body_GetLinearVelocity(_bodyId);
	//vel.x = std::max(vel.x, -_xVelMax);
	//vel.x = std::min(vel.x, _xVelMax);
	//vel.y = std::max(vel.y, -_yVelMax);
	//vel.y = std::min(vel.y, _yVelMax);
	//b2Body_SetLinearVelocity(_bodyId, vel);
}

void DynamicObject::collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB)
{
	RigidObject::collision(with, begin, normal, shapeA, shapeB);

	StaticObject* staticObj = dynamic_cast<StaticObject*>(with);
	KinematicObject* kinematicObj = dynamic_cast<KinematicObject*>(with);

	if ((staticObj || kinematicObj) && begin && normal.y < 0)
		_contactsWithGround.insert(with);
	if ((staticObj || kinematicObj) && !begin)
		_contactsWithGround.erase(with);
}

void DynamicObject::move(Direction dir)
{
	_xDir = dir;

	if (dir == Direction::RIGHT)
		b2Body_ApplyForceToCenter(_bodyId, { _xMoveForce, 0 }, true);
	else if (dir == Direction::LEFT)
		b2Body_ApplyForceToCenter(_bodyId, { -_xMoveForce, 0 }, true);
}

void DynamicObject::jump()
{
	if (onGround())
	{
		printf("apply jump impulse %f\n", _yJumpImpulse);
		b2Body_ApplyLinearImpulseToCenter(_bodyId, { 0, _yJumpImpulse }, true);
	}
}

bool DynamicObject::skidding() const
{
	return onGround() && ((_xDir == Direction::RIGHT && vel().x < 0) ||
		                  (_xDir == Direction::LEFT  && vel().x > 0));
}

bool DynamicObject::onGround() const
{
	return _contactsWithGround.size() > 0;
}