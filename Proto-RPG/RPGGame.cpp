// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "RPGGame.h"
#include "LevelLoader.h"
#include "HUD.h"
#include "Menu.h"

using namespace agp;

RPGGame::RPGGame() : Game("RPG Game", { 500,500 }, 256.0f/224, Game::Rendering::SDL_CPU_SHADERS)
{
	_hud = nullptr;
}

void RPGGame::init()
{
	pushScene(LevelLoader::instance()->load("overworld"));
	_hud = HUD::instance();
	pushScene(_hud);
	pushScene(Menu::mainMenu());
}