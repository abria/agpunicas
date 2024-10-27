// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "TiledSprite.h"
#include "mathUtils.h"
#include <iostream>

using namespace agp;

TiledSprite::TiledSprite(SDL_Texture* spritesheet, const std::vector<RectI>& tiles, Vec2Df tileSize)
	: Sprite(spritesheet, RectI())
{
	_tiles = tiles;
	_tileSize = tileSize;
}

void TiledSprite::render(
	SDL_Renderer* renderer,
	const RectF& drawRect,
	Transform camera,
	const Point& pixelUnitSize,
	float angle,
	SDL_RendererFlip flip)
{
	int tiles_count = 0;

	if (angle)
	{
		std::cerr << "TiledSprite::draw() -> rotation not supported\n";
		return;
	}

	for (float y = drawRect.pos.y; y < drawRect.pos.y + drawRect.size.y; y += _tileSize.y)
		for (float x = drawRect.pos.x; x < drawRect.pos.x + drawRect.size.x && tiles_count < _tiles.size(); x += _tileSize.x)
		{
			SDL_Rect frameRectTile = _tiles[tiles_count++].toSDL();

			RectF tileRect({ x,y }, { x + _tileSize.x,y + _tileSize.y }, drawRect.yUp);
			SDL_FRect drawRectTile = RectF(camera(tileRect.tl()), camera(tileRect.br())).toSDLf();
			SDL_RenderCopyExF(renderer, _spritesheet, &frameRectTile, &drawRectTile, 0, 0, flip);
		}
}