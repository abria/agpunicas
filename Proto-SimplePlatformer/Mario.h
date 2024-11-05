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
#include <map>
#include <string>

namespace agp
{
	class Mario;
	class Sword;
}

class agp::Mario : public DynamicObject
{
	private:

		bool _walking;
		bool _running;
		bool _jumping;
		bool _invincible;
		bool _dying;
		bool _dead;
		bool _attacking;
		double _xLastNonZeroVel;
		Sword* _sword;
		
		std::map<std::string, Sprite*> _sprites;

	public:

		Mario(Scene* scene, const PointF& pos);

		// getters/setters
		bool invincible() { return _invincible; }

		// extends game logic (+mario logic)
		virtual void update(float dt) override;

		// player actions
		virtual void move(Direction dir) override;
		virtual void jump(bool on = true);
		virtual void run(bool on = true);

		// scripted actions
		virtual void attack();
		virtual void die();
		virtual void hurt();

		virtual std::string name() override { return strprintf("Mario[%d]", _id); }
};