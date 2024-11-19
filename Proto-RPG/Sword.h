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
#include "Enemy.h"

namespace agp
{
	class Sword;
	class Link;
}

// Link's sword class.
// - starts brandishing when spawned
class agp::Sword : public DynamicObject
{
	protected:

		Link* _link;

	public:

		Sword(Link* link);
		virtual ~Sword() {}

		// extends game logic (+adapt to Link)
		virtual void update(float dt) override;

		// defines acceptable collisions (enemies only)
		virtual bool collidableWith(CollidableObject* obj) override { return obj->to<Enemy*>(); }

		virtual std::string name() override { 
			return strprintf("Sword[%d]", _id); 
		}
};