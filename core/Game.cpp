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
#include "Scene.h"
#include "GameScene.h"
#include "View.h"
#include "Object.h"
#include "timeUtils.h"
#include "stringUtils.h"
#include "Audio.h"

using namespace agp;

Game::Game(const std::string& windowTitle, const Point& windowSize, float aspectRatio)
{
	_aspectRatio = aspectRatio;
	_scenesToPop = 0;
	_running = false;
	_reset = false;
	_running = false;
	_window = new Window(windowTitle, int(_aspectRatio * windowSize.x), windowSize.y);
	_currentFPS = 0;
}

void Game::run()
{
	_running = true;

	FPS fps;
	Timer <float> frameTimer;

	while (_running)
	{
		processEvents();

		float frameTime = frameTimer.restart();
		for (int i = int(_scenes.size()) - 1; i >= 0; i--)
		{
			_scenes[i]->update(frameTime);
			if (_scenes[i]->blocking())
				break;
		}

		_window->render(_scenes);

		if (fps.update(false))
			_currentFPS = int(round(fps.lastFPS()));
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

	// if there are scenes to be deleted, better to do this after event dispatching
	for (; _scenesToPop > 0; _scenesToPop--)
		popScene();
	
	if (_reset)
	{
		_reset = false;
		for (auto scene : _scenes)
			delete scene;
		_scenes.clear();
		init();
	}
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
}

void Game::pushScene(Scene* scene)
{
	_scenes.push_back(scene);
}

void Game::popScene()
{
	if (_scenes.size())
	{
		delete _scenes.back();
		_scenes.pop_back();
	}
}