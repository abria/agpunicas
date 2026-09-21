// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include "CPUShaderWindow.h"
#include "Scene.h"

using namespace agp;

CPUShaderWindow::CPUShaderWindow(const std::string& title, int width, int height)
	: Window(title, width, height)
{
	_targetTexture = nullptr;
	_GPUBuffer = nullptr;
	_shader = nullptr;
}

CPUShaderWindow::~CPUShaderWindow()
{
	if(_targetTexture)
		SDL_DestroyTexture(_targetTexture);
	if (_GPUBuffer)
		SDL_DestroyTexture(_GPUBuffer);
}

Uint32 CPUShaderWindow::rendererFlags()
{
	return Window::rendererFlags() | SDL_RENDERER_TARGETTEXTURE;
}

void CPUShaderWindow::render(const std::vector<Scene*>& scenes)
{
	// render to target texture first
	SDL_SetRenderTarget(_renderer, _targetTexture);
	SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, 255);
	SDL_RenderClear(_renderer);
	for (auto scene : scenes)
		scene->render();

	// read pixels from target texture (GPU) into CPU buffer
	if (SDL_RenderReadPixels(_renderer, NULL, SDL_PIXELFORMAT_RGBA8888, _CPUBuffer.data(), _width * 4) != 0)
		throw SDL_GetError();

	// apply post-processing on CPU buffer
	if (_shader)
		_shader(reinterpret_cast<Uint32*>(_CPUBuffer.data()), _width, _height, _width * 4);

	// update the streaming texture (GPU) with processed data (CPU)
	SDL_UpdateTexture(_GPUBuffer, NULL, _CPUBuffer.data(), _width * 4);

	// render the processed texture to the window:
	SDL_SetRenderTarget(_renderer, nullptr);
	SDL_RenderClear(_renderer);
	SDL_RenderCopy(_renderer, _GPUBuffer, NULL, NULL);
	SDL_RenderPresent(_renderer);
}

void CPUShaderWindow::resize(int newWidth, int newHeight)
{
	Window::resize(newWidth, newHeight);

	if (_targetTexture)
		SDL_DestroyTexture(_targetTexture);
	_targetTexture = SDL_CreateTexture(
		_renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET,
		_width,
		_height
	);
	if (!_targetTexture)
		throw std::runtime_error(SDL_GetError());

	if (_GPUBuffer)
		SDL_DestroyTexture(_GPUBuffer);
	_GPUBuffer = SDL_CreateTexture(
		_renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		_width,
		_height
	);
	if (!_GPUBuffer)
		throw std::runtime_error(SDL_GetError());

	_CPUBuffer.resize(_width * _height * 4);
}