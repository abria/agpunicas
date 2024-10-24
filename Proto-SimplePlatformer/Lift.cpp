#include "Lift.h"

using namespace agp;

Lift::Lift(Scene* scene, const RectF& rect, Sprite* sprite, bool vertical, float range, int layer) :
	KinematicObject(scene, rect, sprite, layer)
{
	_compenetrable = false;
	_vertical = vertical;
	_r0 = (_vertical ? _rect.pos.y : _rect.pos.x) - range / 2.0f;
	_r1 = (_vertical ? _rect.pos.y : _rect.pos.x) + range / 2.0f;

	if (_vertical)
	{
		_y_gravity = 3.f;
		_y_vel_max = 3;
		_y_vel_min = 0;
	}
	else
	{
		_y_gravity = 0;
		_x_acc = 3.f;
		_x_dec_skd = 3.f;
		_x_vel_max = 3;
		move(Direction::RIGHT);
	}
}

void Lift::update(float dt)
{
	if (_vertical)
	{
		if (_rect.pos.y < _r0)
			_y_gravity = 3.f;
		else if (_rect.pos.y > _r1)
			_y_gravity = -3.f;
	}
	else
	{
		if (_rect.pos.x < _r0)
			move(Direction::RIGHT);
		else if (_rect.pos.x > _r1)
			move(Direction::LEFT);
	}

	KinematicObject::update(dt);
}