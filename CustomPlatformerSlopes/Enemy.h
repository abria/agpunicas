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
	class Mario;
}

// Enemy
// - base class for all enemies
class agp::Enemy : public DynamicObject
{
	protected:

		bool _kickable;
		bool _stompable;
		bool _smashable;
		bool _stomped;
		bool _dying;

		// score points (e.g. after smash, stomp, kick, etc.)
		virtual void score();

	public:

		Enemy(Scene* scene, const RectF& rect, Sprite* sprite, int layer = 2);

		// getters/setters
		virtual bool smashable() const { return _smashable; }

		// actions
		virtual void stomp(Mario* mario);	// mario jumps on top of the enemy
		//virtual void kick(Mario* mario);	// mario kicks from one side
		virtual void smash();				// hit by invincible mario, fireball, shell, or block bump

		// implements behaviors common to all enemies 
		// +smashed
		// +hurt mario
		// +invert direction if hits static or other enemy
		virtual bool collision(CollidableObject* with, bool begin, Direction fromDir) override;

		virtual std::string name() override { return strprintf("Enemy[%d]", _id); }
};