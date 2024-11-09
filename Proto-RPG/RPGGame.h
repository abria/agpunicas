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
	class RPGGame;
	class HUD;
}

// RPGGame
// - customizes parent's class Game to adapt to RPG games
class agp::RPGGame : public Game
{ 
	protected:

		HUD* _hud;

	public: 
		
		RPGGame();
		HUD* hud() { return _hud; }

		virtual void init() override;
		virtual void dialogOptionEntered(const std::string& dialogName, const std::string& option);
};