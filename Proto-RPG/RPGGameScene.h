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
	class RPGGameScene;
	class Link;
	class LevelLoader;
}

// RPGGameScene class
// - customizes parent's class Game to adapt to RPG games
class agp::RPGGameScene : public GameScene
{
	protected:

		// scene transition
		bool _transitionEnter;
		bool _transitionExit;
		float _transitionCounter;

		// helper functions overrides
		virtual void updateControls(float timeToSimulate) override;

	public:

		RPGGameScene(const RectF& rect, const Point& pixelUnitSize, float dt);
		virtual ~RPGGameScene() {};

		// getters/setters
		virtual void setTransitionEnter(bool active);
		virtual void setTransitionExit(bool active);

		// override (+custom game controls)
		virtual void event(SDL_Event& evt) override;

		// override (+hide HUD)
		virtual void displayGameSceneOnly(bool on) override;

		// override (+transitions)
		virtual void update(float timeToSimulate) override;

		// override (+collidable objects only)
		virtual bool isEmpty(const RectF& rect) override;
};