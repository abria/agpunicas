// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Sprite.h"

namespace agp
{
	class FilledSprite;
}

// FilledSprite
// - sprite filled with repetition of the same texture
class agp::FilledSprite : public Sprite
{
	protected:

		Vec2Df _tileSize;				// tile size in sceen coords

	public:

		FilledSprite(SDL_Texture* spritesheet, const RectI& rect, Vec2Df tileSize = { 1,1 });

		// extends render method (+filled)
		virtual void render(
			SDL_Renderer* renderer,
			const RectF& drawRect,
			Transform camera,
			float angle = 0,
			SDL_RendererFlip flip = SDL_FLIP_NONE) override;
};