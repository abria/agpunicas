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
#include "box2d/box2d.h"

namespace agp
{
	class ComplexPlatformerGameScene;
	class Player;
}

// ComplexPlatformerGameScene class
// - customizes parent's class Game to adapt to complex platformer games
class agp::ComplexPlatformerGameScene : public GameScene
{
	protected:

		int _subStepCount = 4;		// Box2D physics substep
		b2WorldId _worldId;			// Box2D world handle

		// helper functions overrides
		virtual void updateControls(float timeToSimulate) override;
		virtual void updateCamera(float timeToSimulate) override;
		virtual void updateWorld(float timeToSimulate) override;

	public:

		ComplexPlatformerGameScene(const RectF& rect, const Point& pixelUnitSize, float dt);
		virtual ~ComplexPlatformerGameScene();

		// getters / setters
		b2WorldId worldId() { return _worldId; }

		// override (+custom game controls)
		virtual void event(SDL_Event& evt) override;
};