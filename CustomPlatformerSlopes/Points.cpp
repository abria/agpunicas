#include "Points.h"
#include "SpriteFactory.h"

using namespace agp;

Points::Points(
	Scene* scene,
	const PointF& pos,
	Points::Value value,
	int layer) :
	MovableObject(
		scene,
		RectF(0, 0, 1, 0.5f).centerOn(pos),
		SpriteFactory::instance()->get(std::string("points_") + char('0' + int(value))), layer)
{
	_yGravityForce = -1;
	_value = value;

	_fit = false;

	schedule("death", 2, [this]() {
		kill();
		});
}