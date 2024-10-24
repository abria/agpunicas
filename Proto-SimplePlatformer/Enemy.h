// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "DynamicObject.h"

namespace agp
{
	class Enemy;
}

// Enemy
// - base class for all enemies
class agp::Enemy : public DynamicObject
{
	protected:

		bool _smashable;
		bool _dying;
		Direction _facingDir;

	public:

		Enemy(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 0);

		Direction facingDir() { return _facingDir; }

		// actions
		//virtual void stomp();					// mario jumps on top of the enemy
		//virtual void kick(bool right = true);	// mario kicks from one side
		virtual void smash();					// hit by invincible mario, fireball, shell, or block bump

		// extends logic collision (+smashed, +hurt Mario)
		virtual bool collision(CollidableObject* with, Direction fromDir) override;

		virtual std::string name() override { return strprintf("Enemy[%d]", _id); }
};