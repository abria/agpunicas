// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "RenderableObject.h"

namespace agp
{
	class Scene;
	class Clipper;
}

// Clipper class.
// - applies clip rect on the screen to all objects drawn on higher layers
class agp::Clipper : public RenderableObject
{
	protected:

		bool _inverted;			// whether to clip objects inside (default=true)
								// or outside (=false) the clipping rect

	public:

		Clipper(Scene* scene, const RectF& rect, int layer = 1000, bool inverted = true)
			: RenderableObject(scene, rect, nullptr, layer){
			_inverted = inverted;
		}
		virtual ~Clipper() {}

		virtual void draw(SDL_Renderer* renderer, Transform camera) override
		{
			SDL_Rect drawRect = RectF(camera(rect().tl()), camera(rect().br())).toSDL();
			SDL_RenderSetClipRect(renderer, &drawRect);

			if (_inverted)
			{
				// Choose the background color or whatever fill is needed
				Color bgColor = _scene->backgroundColor();
				SDL_SetRenderDrawColor(renderer, bgColor.b, bgColor.g, bgColor.r, 255);
				SDL_RenderFillRect(renderer, &drawRect);
				for (auto b : _scene->backgroundImages())
					b->draw(renderer, camera);
			}
		}

		virtual std::string name() override {
			return strprintf("Clipper[%d]", _id);
		}
};