#pragma once

#include "StaticObject.h"

namespace agp
{
	class Coin;
}

// Collectable coin
class agp::Coin : public StaticObject
{
	public:

		Coin(Scene* scene, const PointF& point, int layer = 1);

		// +interaction with Mario
		virtual bool collision(CollidableObject* with, bool begin, Direction fromDir) override;

		virtual std::string name() override {
			return strprintf("Coin[%d]", _id);
		}
};