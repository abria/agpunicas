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
	class OverlayScene;
	class RenderableObject;
	class Sprite;
}

// OverlayScene class
// - draws background or foreground scene with optional parallax effect
// - when constructed, it uses GameScene's view rect size to size its own view rect
class agp::OverlayScene : public Scene
{
	protected:

		GameScene* _gameScene;			// reference GameScene to sync with
		RenderableObject* _foreground;	// foreground sprite (can also be animated)
		Vec2Df _parallaxVel;			// relative velocity in [0,1] w.r.t. GameScene camera (parallax)
		bool _seamless;					// whether the background repeats itself seamlessy

	public:

		OverlayScene(
			GameScene* gameScene,
			Sprite* bgSprite,
			const Vec2Df& parallaxVel = Vec2Df(),
			bool seamless = false);		// if true, bgSprite must be a FilledSprite (repeats itself as needed)
		virtual ~OverlayScene() {};

		// implements parallax logic
		virtual void update(float timeToSimulate) override;
};