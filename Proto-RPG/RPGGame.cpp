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

RPGGame::RPGGame() : Game("RPG Game", { 600,600 }, 256.0f/224)
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

void RPGGame::dialogOptionEntered(const std::string& dialogName, const std::string& option)
{
	printf("Player has entered option \"%s\" from dialog \"%s\"\n", option.c_str(), dialogName.c_str());
}