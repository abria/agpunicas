// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "Enemy.h"

namespace agp
{
	class Hammer;
}

class agp::Hammer : public Enemy
{
	protected:

		Enemy* _thrower;
		bool _throwing;

	public:

		Hammer(Scene* scene, const PointF& pos, Enemy* thrower);

		// extends game logic (+Hammer logic)
		virtual void update(float dt) override;

		// extends collision logic (+collision with Mario only)
		virtual bool collidableWith(CollidableObject* obj) override;

		virtual std::string name() override { return strprintf("Hammer[%d]", _id); }
};