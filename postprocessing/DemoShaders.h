#pragma once

#include "GPUShaderWindow.h"

namespace agp
{
	namespace DemoShaders
	{
		inline const char* const* names()
		{
			static const char* const shaders[] = {
				"rain", "fog", "crt", "pixelate", "neon",
				"heat", "nightvision", "glitch", "bloom", "grayscale"
			};
			return shaders;
		}

		inline void load(GPUShaderWindow* window)
		{
			for (int i = 0; i < 10; ++i)
			{
				const char* name = names()[i];
				window->loadShader(name, std::string("assets/shaders/") + name + ".frag");
				window->removeShader(name);
			}
		}

		inline bool toggle(GPUShaderWindow* window, const SDL_Event& event)
		{
			if (event.type != SDL_EVENT_KEY_DOWN || event.key.repeat)
				return false;

			static const SDL_Scancode keys[] = {
				SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
				SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8,
				SDL_SCANCODE_9, SDL_SCANCODE_0
			};
			for (int i = 0; i < 10; ++i)
			{
				if (event.key.scancode != keys[i])
					continue;
				const char* name = names()[i];
				if (window->shaderActive(name))
					window->removeShader(name);
				else
					window->applyShader(name);
				return true;
			}
			return false;
		}
	}
}
