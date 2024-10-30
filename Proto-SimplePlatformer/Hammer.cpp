#include "Hammer.h"
#include "SpriteFactory.h"
#include "Mario.h"

using namespace agp;

Hammer::Hammer(Scene* scene, const PointF& pos, Enemy* thrower)
	: Enemy(scene, RectF(pos.x, pos.y, 1, 1), SpriteFactory::instance()->get("hammer"))
{
	_collider.adjust(0.2f, 0.2f, -0.2f, -0.2f);
	_smashable = false;
	_thrower = thrower;
	_throwing = false;
	_yGravityForce = 0;
	_xFrictionForce = 0;
	_xSkiddingForce = 0;
	_xMoveForce = 1000;
	_xVelMax = 4;
	_xDir = Direction::NONE;

	schedule("throwing_on", 0.5f, [this]()
		{
			_throwing = true;
			_yGravityForce = 25;
			velAdd(Vec2Df(0, -9));
			_xDir = _thrower->facingDir();
			_angularVelocity = 1000;
		});
}

void Hammer::update(float dt)
{
	Enemy::update(dt);

	if (!_throwing)
		_rect.pos = _thrower->rect().pos + PointF(2 / 16.0f, 0);
}

bool Hammer::collidableWith(CollidableObject* obj)
{
	return dynamic_cast<Mario*>(obj);
}