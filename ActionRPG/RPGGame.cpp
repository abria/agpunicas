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
#include "GPUShaderWindow.h"
#include "DemoShaders.h"

using namespace agp;

RPGGame::RPGGame() : Game("ActionRPG", { 500,500 }, 256.0f/224, Game::Rendering::SDL_GPU_SHADERS)
{
	_hud = nullptr;
}

void RPGGame::init()
{
	auto* shaderWindow = static_cast<GPUShaderWindow*>(window());
	shaderWindow->setShader(nullptr);
	DemoShaders::load(shaderWindow);
	pushScene(LevelLoader::instance()->load("overworld"));
	_hud = HUD::instance();
	pushScene(_hud);
	pushScene(Menu::mainMenu());
}

void RPGGame::dispatchEvent(SDL_Event& evt)
{
	if (DemoShaders::toggle(static_cast<GPUShaderWindow*>(window()), evt))
		return;
	Game::dispatchEvent(evt);
}
