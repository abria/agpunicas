// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <string>
#include "SDL.h"
#include "graphicsUtils.h"

namespace agp
{
	class Window;
	class Scene;
}

// Window (or screen) class
// - stores and initializes renderer system
// - renders the given scenes
class agp::Window
{
	protected:

		SDL_Window* _window;		// SDL window handle
		SDL_Renderer* _renderer;	// SDL renderer handle
		Color _color;				// window attribute
		int _height, _width;		// window attribute
		std::string _title;			// window attribute
		
		// overridable helper functions
		virtual Uint32 windowFlags();
		virtual Uint32 rendererFlags();
		virtual void preWindowCreation() {}
		virtual void initWindow();
		virtual void initRenderer();

	public:

		Window(const std::string& title, int width, int height);
		~Window();

		// init (to be called once after creation)
		virtual void init();

		// getter/setters
		SDL_Renderer* renderer() { return _renderer; }
		void setColor(const Color& c) { _color = c; }

		// render on screen
		virtual void render(const std::vector<Scene*> & scenes);

		// resize
		virtual void resize(int newWidth, int newHeight);
};