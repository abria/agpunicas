// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FilledSprite.h"
#include <iostream>

using namespace agp;

FilledSprite::FilledSprite(SDL_Texture* spritesheet, const RectI& rect, Vec2Df tileSize)
	: Sprite(spritesheet, rect)
{
	_tileSize = tileSize;
}

void FilledSprite::render(SDL_Renderer* renderer, const RectF& drawRect, Transform camera, float angle,			// rotation in degrees, clockwise
	SDL_RendererFlip flip)
{
	SDL_Rect srcRect = _rect.toSDL();

	if (angle)
	{
		std::cerr << "FilledSprite::draw() -> rotation not supported for filled sprites\n";
		return;
	}

	for (float y = drawRect.pos.y; y < drawRect.pos.y + drawRect.size.y; y+= _tileSize.y)
		for (float x = drawRect.pos.x; x < drawRect.pos.x + drawRect.size.x; x += _tileSize.x)
		{
			RectF tileRect({ x,y }, { x + _tileSize.x,y + _tileSize.y }, drawRect.yUp);
			SDL_FRect drawRectTile = RectF(camera(tileRect.tl()), camera(tileRect.br())).toSDLf();
			SDL_RenderCopyExF(renderer, _spritesheet, &srcRect, &drawRectTile, 0, 0, flip);
		}
}