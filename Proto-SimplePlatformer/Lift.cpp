#include "Lift.h"

using namespace agp;

Lift::Lift(Scene* scene, const RectF& rect, Sprite* sprite, bool vertical, float range, int layer) :
	KinematicObject(scene, rect, sprite, layer)
{
	_compenetrable = false;
	_vertical = vertical;
	_r0 = (_vertical ? pos().y : pos().x) - range / 2.0f;
	_r1 = (_vertical ? pos().y : pos().x) + range / 2.0f;

	if (_vertical)
	{
		_yGravityForce = 3.f;
		_yVelMax = 3;
		_yVelMin = 0;
	}
	else
	{
		_yGravityForce = 0;
		_xMoveForce = 3.f;
		_xSkiddingForce = 3.f;
		_xVelMax = 3;
		move(Direction::RIGHT);
	}
}

void Lift::update(float dt)
{
	if (_vertical)
	{
		if (pos().y < _r0)
			_yGravityForce = 3.f;
		else if (pos().y > _r1)
			_yGravityForce = -3.f;
	}
	else
	{
		if (pos().x < _r0)
			move(Direction::RIGHT);
		else if (pos().x > _r1)
			move(Direction::LEFT);
	}

	KinematicObject::update(dt);
}