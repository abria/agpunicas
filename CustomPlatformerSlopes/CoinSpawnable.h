#pragma once

#include "Spawnable.h"

namespace agp
{
	class CoinSpawnable;
}

class agp::CoinSpawnable : public Spawnable
{
	protected:


	public:

		CoinSpawnable(Scene* scene, const PointF& pos, int layer = 1);

		virtual std::string name() override {
			return strprintf("CoinSpawnable[%d]", _id);
		}
};