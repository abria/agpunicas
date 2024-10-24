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
	_y_gravity = 0;
	_x_dec_rel = 0;
	_x_dec_skd = 0;
	_x_acc = 1000;
	_x_vel_max = 4;
	_x_dir = Direction::NONE;

	schedule("throwing_on", 0.5f, [this]()
		{
			_throwing = true;
			_y_gravity = 25;
			velAdd(Vec2Df(0, -9));
			_x_dir = _thrower->facingDir();
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