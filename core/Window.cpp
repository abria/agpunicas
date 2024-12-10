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
#include "stringUtils.h"

using namespace agp;

Window::Window(const std::string& title, int width, int height)
{
	_window = nullptr;
	_renderer = nullptr;
	_title = title;
	_color = Color(128, 128, 128);
	_width = width;
	_height = height;

	if (SDL_Init(SDL_INIT_VIDEO))
		throw SDL_GetError();
}

void Window::init()
{
	preWindowCreation();
	initWindow();
	initRenderer();
}

Window::~Window()
{
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
}

Uint32 Window::rendererFlags()
{
	return SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
}

Uint32 Window::windowFlags()
{
	return SDL_WINDOW_RESIZABLE;
}

void Window::initWindow()
{
	_window = SDL_CreateWindow(
		_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		_width,
		_height,
		windowFlags()
	);
	if (!_window)
		throw SDL_GetError();
}

void Window::initRenderer()
{
	_renderer = SDL_CreateRenderer(
		_window,
		-1,
		rendererFlags()
	);
	if (!_renderer)
		throw SDL_GetError();

	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);

	resize(_width, _height);
}

void Window::render(const std::vector<Scene*>& scenes)
{
	SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, 255);
	SDL_RenderClear(_renderer);

	for (auto scene : scenes)
		scene->render();

	SDL_RenderPresent(_renderer);
}

void Window::resize(int newWidth, int newHeight)
{
	_width = newWidth;
	_height = newHeight;

	SDL_SetWindowSize(_window, _width, _height);
}