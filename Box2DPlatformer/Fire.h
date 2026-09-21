#pragma once

#include "DynamicObject.h"

namespace agp
{
	class Fire;
}

class agp::Fire : public DynamicObject
{
	private:

	public:

		Fire(GameScene* scene, const PointF& spawnPoint, const Vec2Df& velocity, int layer=0);

		virtual void update(float dt) override;
		virtual std::string name() override { return strprintf("Fire[%d]", _id); }
};