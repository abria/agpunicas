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
	class Link;
}

// Link's sword class.
class agp::Sword : public DynamicObject
{
	protected:

		Link* _link;

	public:

		Sword(Link* link);
		virtual ~Sword() {}

		// extends game logic (+adapt to Link)
		virtual void update(float dt) override;

		virtual std::string name() override { 
			return strprintf("Sword[%d]", _id); 
		}
};