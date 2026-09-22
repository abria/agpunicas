// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#ifdef WITH_SHADERS

#pragma once
#include "Window.h"
#include "CPUShaderWindow.h"
#include <map>
#include <string>
#include <vector>

namespace agp
{
	class GPUShaderWindow;
}

// GPUShaderWindow class
// - adds GPU fragment shaders frame post-processing to Window's rendering pipeline
class agp::GPUShaderWindow : public agp::Window
{
	protected:

		struct ShaderPass
		{
			SDL_GPUShader* shader = nullptr;
			SDL_GPURenderState* state = nullptr;
		};

		SDL_GPUDevice* _device;
		SDL_Texture* _sceneTarget;
		SDL_Texture* _effectTarget;
		SDL_Texture* _cpuResult;
		int _targetWidth;
		int _targetHeight;
		Uint64 _startTime;
		std::map<std::string, ShaderPass> _shaders;
		std::vector<std::string> _activeShaders;
		CPUShader _cpuShader;

		// helper functions
		virtual void initRenderer() override;
		void ensureTargets();

	public:

		GPUShaderWindow(const std::string& title, int width, int height);
		virtual ~GPUShaderWindow();

		void loadShader(const std::string& name, const std::string& assetPath);
		void applyShader(const std::string& name);
		void removeShader(const std::string& name);
		bool shaderActive(const std::string& name) const;
		void setShader(CPUShader shader) { _cpuShader = shader; }

		// override (+GPU shader)
		virtual void render(const std::vector<Scene*>& scenes) override;
};

#endif