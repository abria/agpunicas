// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include "Game.h"
#include "Window.h"
#include "GameScene.h"
#include "View.h"
#include "Object.h"
#include "timeUtils.h"
#include "stringUtils.h"
#include "SpriteFactory.h"
#include "LevelLoader.h"
#include "HUD.h"
#include "Menu.h"

using namespace agp;

Game* Game::instance()
{
	static Game uniqueInstance;
	return &uniqueInstance;
}

Game::Game()
{
	_scenesToPop = 0;
	_running = false;
	_window = new Window("Proto-ComplexPlatformer-Box2D", int(aspectRatio() * 600), 600);
	_hud = nullptr;
}

void Game::init()
{
	auto gameScenes = LevelLoader::instance()->load("level0");
	for (auto& scene : gameScenes)
		pushScene(scene);

	_hud = new HUD();
	pushScene(_hud);
	pushScene(Menu::mainMenu());
}

void Game::run()
{
	init();
	_running = true;

	FPS fps;
	Timer <float> frameTimer;

	while (_running)
	{
		processEvents();

		float frameTime = frameTimer.restart();
		for (auto& layer : _scenes)
			layer->update(frameTime);

		_window->render(_scenes);

		//fps.update(true);
		if(fps.update(false))
			_hud->setFPS(int(round(fps.lastFPS())));
	}

	destroy();
}

void Game::destroy()
{
	for (auto scene : _scenes)
		delete scene;

	if(_window)
		delete _window;

	SDL_Quit();
}

void Game::processEvents()
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
		dispatchEvent(evt);
}

void Game::dispatchEvent(SDL_Event& evt)
{
	// window events are dispatched to all scenes for their views adjustments
	if (evt.type == SDL_WINDOWEVENT)
		for (auto& scene : _scenes)
			scene->event(evt);

	// all other events are dispatched from top to down through the scene stack
	// if a blocking layer is encountered, event propagation stops
	else if (_scenes.size())
	{
		for (int i = int(_scenes.size()) - 1; i >= 0; i--)
		{
			_scenes[i]->event(evt);
			if (_scenes[i]->blocking())
				break;
		}
	}

	// if there are scenes to be deleted, better to do this after event dispatching
	for (; _scenesToPop > 0; _scenesToPop--)
		popScene();
}

void Game::pushScene(Scene* scene)
{
	if(scene->blocking() && _scenes.size())
		for(auto & scene : _scenes)
			scene->setActive(false);

	_scenes.push_back(scene);
}

void Game::popScene()
{
	if (_scenes.size())
	{
		// last scene is blocking lower scenes
		if (_scenes.back()->blocking())
		{
			// reactivate lower scenes down to first blocking scene
			for (int i = int(_scenes.size()) - 2; i >= 0; i--)
			{
				_scenes[i]->setActive(true);
				if (_scenes[i]->blocking())
					break;
			}
		}

		delete _scenes.back();
		_scenes.pop_back();
	}
}