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

#include "SpriteFactory.h"
#include "Audio.h"
#include "Window.h"

#ifdef WITH_TTF
#include "Fonts.h"
#endif

using namespace agp;

RPGGame::RPGGame() : Game("ActionRPG", { 500,500 }, 256.0f/224, Game::Rendering::SDL_GPU_SHADERS)
{
	_hud = nullptr;

	try
	{
		SpriteFactory::instance()->init(window()->renderer());
		Audio::instance();
#ifdef WITH_TTF
		Fonts::instance();
#endif
	}
	catch (...)
	{
		// A failed constructor does not run this class's destructor.
		releaseServices();
		throw;
	}
}

RPGGame::~RPGGame()
{
	// Scenes must be destroyed before the resources they use.
	clearScenes();
	HUD::destroyInstance();
	releaseServices();
}

void RPGGame::releaseServices()
{
#ifdef WITH_TTF
	Fonts::destroyInstance();
#endif
	Audio::destroyInstance();
	SpriteFactory::destroyInstance();
}

void RPGGame::init()
{
	auto* shaderWindow = static_cast<GPUShaderWindow*>(window());
	shaderWindow->setShader(nullptr);
	DemoShaders::load(shaderWindow);
	pushScene(LevelLoader::load("overworld"));
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
