// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "GameScene.h"

namespace agp
{
	class PlatformerGameScene;
	class Link;
}

// PlatformerGameScene class
// - customizes parent's class Game to adapt to simple platformer games
class agp::PlatformerGameScene : public GameScene
{
	protected:

		// helper functions overrides
		virtual void updateControls(float timeToSimulate) override;
		virtual void updateCamera(float timeToSimulate) override;

	public:

		PlatformerGameScene(const RectF& rect, const Point& pixelUnitSize, float dt);
		virtual ~PlatformerGameScene() {};

		// override (+custom game controls)
		virtual void event(SDL_Event& evt) override;
};