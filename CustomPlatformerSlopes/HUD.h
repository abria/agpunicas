// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "UIScene.h"
#include "RenderableObject.h"

namespace agp
{
	class HUD;
}

// HUD
// - implements SMB3's HUD
class agp::HUD : public UIScene
{
	protected:

		// actual data
		int _score;
		int _coins;
		int _world;
		int _lives;
		float _time;
		int _pMeter;
		bool _running;
		float _runningTime;
		float _notRunningTime;
		int _comboCount;
		float _comboTimer;
		const float COMBO_WINDOW = 1.0f;
		int _fps;

		// rendering objects
		RenderableObject* _scoreObj;
		RenderableObject* _coinsObj;
		RenderableObject* _livesObj;
		RenderableObject* _worldObj;
		RenderableObject* _timeObj;
		RenderableObject* _fpsObj;
		RenderableObject* _pMeterObjects[7];

	public:

		HUD();
		virtual ~HUD() {};

		// getters/setters
		void setFPS(int fps);
		void setRunning(bool on);
		bool pSpeed() const { return _pMeter == 7; }

		void addCoin();
		void addLife();
		void addScore(int score);
		int addKill();	// returns combo index (from 0 to 8)

		// extends update logic (+time management)
		virtual void update(float timeToSimulate) override;
};