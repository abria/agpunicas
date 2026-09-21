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
	class NPC;
	class Sword;
}

class agp::NPC : public DynamicObject
{
	protected:

		// you can put here interaction data
		// (dialogs, texts, options, actions)

	public:

		NPC(Scene* scene, const PointF& pos);

		virtual void interact();

		virtual std::string name() override { return strprintf("NPC[%d]", _id); }
};