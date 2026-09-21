// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "geometryUtils.h"
#include <vector>
#include "Game.h"

namespace agp
{
	class ComplexPlatformerGame;
	class HUD;
}

// ComplexPlatformerGame
// - customizes parent's class Game to adapt to complex platformer games
class agp::ComplexPlatformerGame : public Game
{ 
	protected:

		HUD* _hud;

	public: 
		
		ComplexPlatformerGame();
		HUD* hud() { return _hud; }

		virtual void init() override;
};
