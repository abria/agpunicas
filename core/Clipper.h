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
	public:

		Clipper(Scene* scene, const RectF& rect, int layer = 0)
			: RenderableObject(scene, rect, nullptr, layer){}
		virtual ~Clipper() {}

		virtual void draw(SDL_Renderer* renderer, Transform camera)
		{
			// get renderer size on screen
			int rendWidth, rendHeight;
			SDL_GetRendererOutputSize(renderer, &rendWidth, &rendHeight);

			// clipRect in screen coordinates
			SDL_Rect clipRect = {
				int(_rect.pos.x * rendWidth),
				int(_rect.pos.y * rendHeight),
				int(_rect.size.x * rendWidth),
				int(_rect.size.y * rendHeight) };

			SDL_RenderSetClipRect(renderer, &clipRect);
		}

		virtual std::string name() override {
			return strprintf("Clipper[%d]", _id);
		}
};