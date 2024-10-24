// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Scene.h"

namespace agp
{
	class UIScene;
}

// UIScene (or GUI) class
// - specialized update(dt) to variable, framerate-dependent timestep
class agp::UIScene : public Scene
{
	public:

		UIScene(const RectF& r);
		virtual ~UIScene() {};

		// implements UI scene update logic (+variable timestep)
		virtual void update(float timeToSimulate) override;
};