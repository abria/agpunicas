// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PlatformerGame.h"
#include "PlatformerGameScene.h"
#include "Scene.h"
#include "LevelLoader.h"
#include "HUD.h"
#include "Menu.h"
#include "Audio.h"
#include "SpriteFactory.h"
#include "UIScene.h"
#include "GPUShaderWindow.h"
#include "DemoShaders.h"

using namespace agp;

PlatformerGame::PlatformerGame() : Game("CustomPlatformer", { 600,600 }, 16/14.0f, Rendering::SDL_GPU_SHADERS)
{
	_hud = nullptr;
}

void PlatformerGame::init()
{
	DemoShaders::load(static_cast<GPUShaderWindow*>(window()));

	pushScene(LevelLoader::instance()->load("overworld"));
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

void PlatformerGame::freeze(bool on)
{
	PlatformerGameScene* gameScene = dynamic_cast<PlatformerGameScene*>(_scenes[0]);
	if (gameScene)
	{
		Objects objects = gameScene->objects();
		for (auto& obj : objects)
			if (obj != reinterpret_cast<Object*>(gameScene->player()))
				obj->setFreezed(on);
	}
}

void PlatformerGame::gameover()
{
	UIScene* gameoverScreen = new UIScene(RectF(0, 0, 16, 15), { 16, 16 });
	gameoverScreen->setBackgroundColor(Color(0, 0, 0));
	new RenderableObject(gameoverScreen, RectF(0, 0, 16, 15), SpriteFactory::instance()->get("gameover"));
	pushScene(gameoverScreen);
	Audio::instance()->playMusic("gameover", 0);
}
