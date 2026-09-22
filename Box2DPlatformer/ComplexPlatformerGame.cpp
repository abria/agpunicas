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
#include "GPUShaderWindow.h"
#include "DemoShaders.h"

using namespace agp;

ComplexPlatformerGame::ComplexPlatformerGame() : Game("Box2DPlatformer", { 600,600 }, 1.846f, Rendering::SDL_GPU_SHADERS)
{
	_hud = nullptr;
}

void ComplexPlatformerGame::init()
{
	DemoShaders::load(static_cast<GPUShaderWindow*>(window()));
	pushScene(LevelLoader::instance()->load("level0"));
	_hud = new HUD();
	pushScene(_hud);
	pushScene(Menu::mainMenu());
}

void ComplexPlatformerGame::dispatchEvent(SDL_Event& evt)
{
	if (DemoShaders::toggle(static_cast<GPUShaderWindow*>(window()), evt))
		return;
	Game::dispatchEvent(evt);
}
