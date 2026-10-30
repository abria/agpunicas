// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PlatformerGame.h"
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

PlatformerGame::PlatformerGame() : Game("Box2DPlatformer", { 600,600 }, 1.846f, Rendering::SDL_GPU_SHADERS)
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

PlatformerGame::~PlatformerGame()
{
	// Scenes must be destroyed before the resources they use.
	clearScenes();
	releaseServices();
}

void PlatformerGame::releaseServices()
{
#ifdef WITH_TTF
	Fonts::destroyInstance();
#endif
	Audio::destroyInstance();
	SpriteFactory::destroyInstance();
}

void PlatformerGame::init()
{
	DemoShaders::load(static_cast<GPUShaderWindow*>(window()));
	pushScene(LevelLoader::load("level0"));
	_hud = new HUD();
	pushScene(_hud);
	pushScene(Menu::mainMenu());
}

void PlatformerGame::dispatchEvent(SDL_Event& evt)
{
	if (DemoShaders::toggle(static_cast<GPUShaderWindow*>(window()), evt))
		return;
	Game::dispatchEvent(evt);
}
