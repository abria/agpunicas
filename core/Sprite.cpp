// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Sprite.h"
#include <iostream>

using namespace agp;

Sprite::Sprite(SDL_Texture* spritesheet, const RectI& rect)
{
	_spritesheet = spritesheet;
	_rect = rect;

	if (!_rect.isValid())
		SDL_QueryTexture(spritesheet, nullptr, nullptr, &_rect.size.x, &_rect.size.y);
}

void Sprite::render(
	SDL_Renderer* renderer, 
	const RectF& drawRect, 
	Transform camera, 
	const Point& pixelUnitSize, 
	float angle,
	SDL_RendererFlip flip,
	bool fit)
{
	SDL_Rect srcRect = _rect.toSDL();
	SDL_FRect drawRect_sdl;

	// expand
	if (!fit)
	{
		// correct aspect ratio
		RectF correctedDrawRectAR = drawRect;
		correctedDrawRectAR.size.y = drawRect.size.x / _rect.aspectRatio();

		// correct scale mismatch (might be due to previous AR correction) 
		RectF pixelRect(0, 0, 1.0f / pixelUnitSize.x, 1.0f / pixelUnitSize.y);
		RectF scaledPixelRect(camera(pixelRect.tl()), camera(pixelRect.br()));
		RectF scaledCorrectedDrawRectAR(camera(correctedDrawRectAR.tl()), camera(correctedDrawRectAR.br()));
		Vec2Df scaleCorrection = (scaledCorrectedDrawRectAR.size / _rect.size) / scaledPixelRect.size;
		scaledCorrectedDrawRectAR.size /= scaleCorrection;

		// correct position
		RectF scaledDrawRect(camera(drawRect.tl()), camera(drawRect.br()));
		if (flip & SDL_FLIP_HORIZONTAL)
			scaledCorrectedDrawRectAR.pos.x -= scaledCorrectedDrawRectAR.size.x - scaledDrawRect.size.x;
		scaledCorrectedDrawRectAR.pos.y -= scaledCorrectedDrawRectAR.size.y - scaledDrawRect.size.y;

		drawRect_sdl = scaledCorrectedDrawRectAR.toSDLf();
	}
	else 
		drawRect_sdl = RectF(camera(drawRect.tl()), camera(drawRect.br())).toSDLf();

	SDL_RenderCopyExF(renderer, _spritesheet, &srcRect, &drawRect_sdl, -angle, 0, flip);
}