#pragma once

#include "MovableObject.h"

namespace agp
{
	class Points;
}

// Points 
class agp::Points : public MovableObject
{
	public:

		enum class Value { P100, P200, P400, P800, P1000, P2000, P4000, P8000, LIFE };

	protected:

		Points::Value _value;

	public:

		Points(
			Scene* scene, 
			const PointF& pos,
			Points::Value value,
			int layer = 2);

		virtual std::string name() override {
			return strprintf("Points[%d]", _id);
		}
};