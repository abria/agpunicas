#include "Platform.h"
#include "DynamicObject.h"

using namespace agp;

Platform::Platform(Scene* scene, const LineF& line, int layer) :
	StaticObject(scene, line.boundingRect(false), nullptr, layer)
{

}

bool Platform::collidableWith(CollidableObject* obj)
{
	auto* body = obj->to<DynamicObject*>();
	// The CCD broad phase temporarily moves the body forward. Use its position
	// at the start of the step so a downward crossing is still accepted.
	return body && body->vel().y >= 0 &&
		body->movementStartCollider().bottom() <= sceneCollider().top() + 0.001f;
}

bool Platform::sweep(const RectF& body, const Vec2Df& movement,
	Vec2Df& point, Vec2Df& normal, float& time) const
{
	return movement.y > 0 &&
		StaticObject::sweep(body, movement, point, normal, time) && normal.y < 0;
}

bool Platform::overlap(const RectF& body, Direction& axis, float& depth) const
{
	const RectF platform = sceneCollider();
	if (body.right() <= platform.left() || body.left() >= platform.right() ||
		body.bottom() < platform.top() || body.top() > platform.top())
		return false;
	// Only a vertical response, including at the ends of the segment.
	axis = Direction::DOWN;
	depth = body.bottom() - platform.top();
	return true;
}

