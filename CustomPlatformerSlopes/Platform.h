#pragma once

#include "StaticObject.h"

namespace agp
{
	class Platform;
}

// One-way horizontal platform: dynamic objects collide only when landing on top.
class agp::Platform : public StaticObject
{
	protected:

		

	public:

		Platform(Scene* scene, const LineF& line, int layer = 0);

		bool collidableWith(CollidableObject* obj) override;
		bool sweep(const RectF& body, const Vec2Df& movement,
			Vec2Df& point, Vec2Df& normal, float& time) const override;
		bool overlap(const RectF& body, Direction& axis, float& depth) const override;

		virtual std::string name() override {
			return strprintf("Platform[%d]", _id);
		}
};
