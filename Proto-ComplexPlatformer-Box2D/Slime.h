// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "Enemy.h"

namespace agp
{
	class Slime;
}

class agp::Slime : public Enemy
{
	protected:

	public:

		Slime(GameScene* scene, const PointF& pos);

		virtual std::string name() override { return strprintf("Slime[%d]", _id); }
};