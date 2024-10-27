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
	class Link;
}

class agp::Link : public DynamicObject
{
	private:

		// state flags
		bool _walking;
		bool _attacking;
		
		// sprites (1 set for each facing direction)
		std::map<std::string, Sprite*> _sprites[4];

		// decorations
		RenderableObject* _shadow;
		RenderableObject* _shields[4];

	public:

		Link(Scene* scene, const PointF& pos);

		// extends game logic (+mario logic)
		virtual void update(float dt) override;

		// scripted actions
		virtual void attack();
		virtual void die();
		virtual void hurt();

		virtual std::string name() override { return strprintf("Link[%d]", _id); }
};