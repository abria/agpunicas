#pragma once

#include "DynamicObject.h"

namespace agp
{
	class Box;
}

class agp::Box : public DynamicObject
{
	private:

	public:

		Box(GameScene* scene, const RotatedRectF& obb);

		virtual std::string name() override { return strprintf("Box[%d]", _id); }
};