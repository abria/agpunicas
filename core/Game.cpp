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
#include "GPUShaderWindow.h"
#include "CPUShaderWindow.h"

using namespace agp;

Game::Game(
	const std::string& windowTitle, 
	const Point& windowSize, 
	float aspectRatio,
	Rendering rendering)
{
	_aspectRatio = aspectRatio;
	_scenesToPop = 0;
	_running = false;
	_reset = false;
	_running = false;
	_timeScaleIndex = 4;
	_timeScales = { 0.05f, 0.1f, 0.2f, 0.5f, 1.0f, 2.0f, 4.0f, 10.0f, 20.0f };

	if (rendering == Rendering::SDL)
		_window = new Window(windowTitle, int(_aspectRatio * windowSize.x), windowSize.y);
	else if (rendering == Rendering::SDL_CPU_SHADERS)
		_window = new CPUShaderWindow(windowTitle, int(_aspectRatio * windowSize.x), windowSize.y);
	else if (rendering == Rendering::SDL_GPU_SHADERS)
#ifdef WITH_SHADERS
		_window = new GPUShaderWindow(windowTitle, int(_aspectRatio * windowSize.x), windowSize.y);
#else
		throw "GPUShaderWindow not supported, you need to activate WITH_SHADERS at CMake time";
#endif
	_window->init();
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
		if (!_running)
			break;

		float frameTime = frameTimer.restart();
		for (int i = int(_scenes.size()) - 1; i >= 0; i--)
		{
			_scenes[i]->update(frameTime *_timeScales[_timeScaleIndex]);
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
	if (evt.type == SDL_EVENT_QUIT || evt.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
	{
		quit();
		return;
	}

	if (evt.type == SDL_EVENT_KEY_DOWN && evt.key.scancode == SDL_SCANCODE_ESCAPE)
	{
		quit();
		return;
	}

	// detect CTRL and SHIFT modifiers
	const bool* keyboardState = SDL_GetKeyboardState(0);
	bool ctrlPressed = keyboardState[SDL_SCANCODE_LCTRL] || keyboardState[SDL_SCANCODE_RCTRL];
	bool shiftPressed = keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT];


	// window events are dispatched to all scenes for their views adjustments
	if (evt.type >= SDL_EVENT_WINDOW_FIRST && evt.type <= SDL_EVENT_WINDOW_LAST)
	{
		if (evt.type == SDL_EVENT_WINDOW_RESIZED)
			_window->resize(evt.window.data1, evt.window.data2);

		for (auto& scene : _scenes)
			scene->event(evt);
	}

	// game speed regulation events
	else if (ctrlPressed && shiftPressed && evt.type == SDL_EVENT_KEY_DOWN && evt.key.scancode == SDL_SCANCODE_UP && !evt.key.repeat)
	{
		_timeScaleIndex = std::min(_timeScaleIndex + 1, int(_timeScales.size()) - 1);
		printf("Game speed %.2f\n", _timeScales[_timeScaleIndex]);
	}
	else if (ctrlPressed && shiftPressed && evt.type == SDL_EVENT_KEY_DOWN && evt.key.scancode == SDL_SCANCODE_DOWN && !evt.key.repeat)
	{
		_timeScaleIndex = std::max(_timeScaleIndex - 1, 0);
		printf("Game speed %.2f\n", _timeScales[_timeScaleIndex]);
	}

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
