// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Window.h"
#include <functional>
#include "graphicsUtils.h"

namespace agp
{
	class CPUShaderWindow;
	class Scene;

	// CPU shader-equivalent image processing function prototype
	// inputs: pointer to pixel data (Uint32*), width, height, and pitch (bytes per row)
	typedef std::function<void(Uint32*, int, int, int)> CPUShader;
}

// CPUShaderWindow class
// - adds frame postprocessing (shader) to Window's rendering pipeline
class agp::CPUShaderWindow : public agp::Window
{
	protected:

		SDL_Texture* _targetTexture;		// render-to-texture target
		SDL_Texture* _GPUBuffer;			// post-processing GPU buffer
		std::vector<Uint8> _CPUBuffer;		// post-processing CPU buffer
		CPUShader _shader;					// frame post-processing function

		virtual Uint32 rendererFlags() override;

	public:

		CPUShaderWindow(const std::string& title, int width, int height);
		~CPUShaderWindow();

		// getter/setters
		void setShader(CPUShader shader) { _shader = shader; }

		// override (+CPU shader)
		virtual void render(const std::vector<Scene*> & scenes) override;

		// override (+texture reallocation)
		virtual void resize(int newWidth, int newHeight) override;
};