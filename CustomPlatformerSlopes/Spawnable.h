#pragma once

#include "DynamicObject.h"

namespace agp
{
	class Spawnable;
	class Scene;
}

// Abstract class for all spawnable items
class agp::Spawnable : public DynamicObject
{
	public:

		enum class Type { COIN, POWERUP, LIFE, STARMAN, NONE };

	protected:

		bool _bounceable;
		Spawnable(Scene* scene, const RectF& rect, int layer = 1);

	public:

		static Spawnable* spawn(Scene* scene, Spawnable::Type type, const PointF& pos, CollidableObject* spawner);

		// getters/setters
		virtual bool bounceable() const { return _bounceable; }

		virtual std::string name() override {
			return strprintf("Spawnable[%d]", _id);
		}
};