// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "geometryUtils.h"
#include "Singleton.h"
#include <vector>

namespace agp
{
	class Game;
	class Scene;
	class Window;
}

// Game (singleton)
// - implements game loop
// - contains the scenes stack
// - receives and dispatches events throughout scene stack
// - singleton access
class agp::Game : public Singleton<Game>
{ 
	friend class Singleton<Game>;

	public:

		//typedef std::set< Scene*> SceneSet;

	protected:

		// attributes
		Window* _window;
		float _aspectRatio;					// -1 if free to vary
		std::vector<Scene*> _scenes;		// scenes stack
		int _scenesToPop;					// for popSceneLater
		bool _running;
		bool _reset;
		int _currentFPS;

		// helper functions
		virtual void destroy();
		virtual void processEvents();

	public: 
		
		Game(const std::string& windowTitle = "Game", const Point& windowSize = { 600,600 }, float aspectRatio = -1);

		// getters
		Window* window() { return _window; }
		float aspectRatio() { return _aspectRatio; }
		int currentFPS() { return _currentFPS; }

		// scene stack access
		void pushScene(Scene* scene);
		void popScene();
		void popSceneLater() { _scenesToPop++; }

		// event dispatcher
		virtual void dispatchEvent(SDL_Event& evt);

		// initialization (e.g. menus, scenes)
		virtual void init() {};

		// runs the game loop until the game is over
		virtual void run();

		// game change states
		virtual void quit() { _running = false; }
		virtual void reset() { _reset = true; }
};