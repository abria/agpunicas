// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include "Window.h"
#include "View.h"
#include "Scene.h"

#ifdef WITH_TTF
#include "SDL_ttf.h"
#endif

using namespace agp;

Window::Window(const std::string& title, int width, int height)
{
	_window = nullptr;
	_renderer = nullptr;
	_title = title;
	_color = Color(128, 128, 128);

	if (SDL_Init(SDL_INIT_VIDEO))
		throw SDL_GetError();

	_window = SDL_CreateWindow(
		_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_RESIZABLE
	);
	if(!_window)
		throw SDL_GetError();

	_renderer = SDL_CreateRenderer(
		_window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if(!_renderer)
		throw SDL_GetError();

	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

#ifdef WITH_TTF
	TTF_Init();
#endif
}

Window::~Window()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);

#ifdef WITH_TTF
	TTF_Quit();
#endif
}

void Window::render(const std::vector<Scene*>& scenes)
{
	SDL_SetRenderDrawColor(_renderer, _color.r, _color.b, _color.g, 255);
	SDL_RenderClear(_renderer);

	for(auto scene : scenes)
		scene->render();

	SDL_RenderPresent(_renderer);
}