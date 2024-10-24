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
#include <vector>

namespace agp
{
	class TiledSprite;
}

// TiledSprite
// - implements composite (tiled) sprite
class agp::TiledSprite : public Sprite
{
	protected:

		std::vector<RectI> _tiles;		// row-wise sprite tiles
		Vec2Df _tileSize;				// tile size in sceen coords

	public:

		TiledSprite(SDL_Texture* spritesheet, const std::vector<RectI>& tiles, Vec2Df tileSize = {1,1});

		// extends render method (+composite)
		virtual void render(
			SDL_Renderer* renderer, 
			const RectF& drawRect, 
			Transform camera, 
			float angle = 0,			
			SDL_RendererFlip flip = SDL_FLIP_NONE) override;
};