// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Game.h"

namespace agp
{
	class PlatformerGame;
	class HUD;
}

// PlatformerGame
// - customizes parent's class Game to adapt to simple platformer games
class agp::PlatformerGame : public Game
{ 
	protected:

		HUD* _hud;

	public: 
		
		PlatformerGame();
		HUD* hud() { return _hud; }

		virtual void init() override;

		void freeze(bool on);
		void gameover();
};