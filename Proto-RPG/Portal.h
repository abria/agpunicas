// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <functional>
#include "Trigger.h"

namespace agp
{
	class Portal;
}

// Portal class
// -  teleports player to another portal
class agp::Portal : public Trigger
{
	protected:

		bool _playerArrived;	// true when player has been teleported to this portal

	public:

		Portal(Scene* scene, const RectF& rect, Portal* dest = nullptr);

		void setDestination(Portal *dest);

		// extends logic collision (+portal behavior)
		virtual bool collision(CollidableObject* with, bool begin, Direction fromDir) override;
		
		virtual std::string name() override {
			return strprintf("Portal[%d]", _id);
		}
};