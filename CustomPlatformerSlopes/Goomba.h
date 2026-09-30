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
	class Goomba;
	class Mario;
}

// Enemy
// - base class for all enemies
class agp::Goomba : public Enemy
{
	protected:

		Sprite* _spriteStomped;

	public:

		Goomba(Scene* scene, const PointF& pos, int layer = 2);

		// @override: die if stomped
		virtual void stomp(Mario* mario) override;
		
		// @override: +ignore collisions with dynamic objects if stomped
		virtual bool collidableWith(CollidableObject* obj) override;

		virtual std::string name() override { return strprintf("Goomba[%d]", _id); }
};