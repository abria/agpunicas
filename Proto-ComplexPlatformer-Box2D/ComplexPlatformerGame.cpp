// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ComplexPlatformerGame.h"
#include "LevelLoader.h"
#include "HUD.h"
#include "Menu.h"

using namespace agp;

ComplexPlatformerGame::ComplexPlatformerGame() : Game("Complex Platformer Game", { 600,600 }, 1.846f)
{
	_hud = nullptr;
}

void ComplexPlatformerGame::init()
{
	pushScene(LevelLoader::instance()->load("level0"));
	_hud = new HUD();
	pushScene(_hud);
	pushScene(Menu::mainMenu());
}