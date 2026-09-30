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
}

class agp::Mario : public DynamicObject
{
	private:

		bool _walking;
		bool _running;
		bool _jumping;
		bool _canJump;
		bool _skidding;
		bool _invincible;
		bool _dying;
		bool _dead;
		double _xLastNonZeroVel;
		bool _transformSmall2Super;
		bool _transformSuper2Small;
		bool _super;
		bool _crouch;
		bool _pSpeed;
		
		std::map<std::string, Sprite*> _sprites[2];	// 0 = small mario; 1 = supermario

	public:

		Mario(Scene* scene, const PointF& pos);

		// getters/setters
		bool invincible() { return _invincible; }
		bool flashing() { return flashingFrequency() != 0; }
		bool super() { return _super; }

		// extends game logic (+mario logic)
		virtual void update(float dt) override;

		// player actions
		virtual void move(Direction dir) override;
		virtual void jump(bool on = true);
		virtual void bounce();
		virtual void run(bool on = true);
		virtual void crouch(bool on = true);

		// scripted actions
		virtual void die();
		virtual void hurt();

		// transformations
		virtual void powerup(bool start = true);
		virtual void powerdown(bool start = true);


		virtual std::string name() override { return strprintf("Mario[%d]", _id); }
};