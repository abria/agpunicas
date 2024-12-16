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
	class Sword;
	class Mario;
}

// Link's sword class.
class agp::Sword : public DynamicObject
{
	protected:

		Mario* _link;

	public:

		Sword(Mario* link);
		virtual ~Sword() {}

		// extends game logic (+adapt to Link)
		virtual void update(float dt) override;

		// extends logic collision (+hurt enemies)
		virtual bool collision(CollidableObject* with, bool begin, Direction fromDir) override;

		// extends collision logic (+collision with Enemy only)
		virtual bool collidableWith(CollidableObject* obj) override;

		virtual std::string name() override { 
			return strprintf("Sword[%d]", _id); 
		}
};