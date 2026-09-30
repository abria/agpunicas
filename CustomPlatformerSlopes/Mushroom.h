#pragma once

#include "Spawnable.h"

namespace agp
{
	class Mushroom;
}

class agp::Mushroom : public Spawnable
{
	protected:

		bool _isGreen;

	public:

		Mushroom(Scene* scene, const PointF& pos, bool isGreen, int layer = 1);

		// @override: +collision with Mario
		virtual bool collision(CollidableObject* with, bool begin, Direction fromDir) override;


		virtual std::string name() override {
			return strprintf("Mushroom[%d]", _id);
		}
};