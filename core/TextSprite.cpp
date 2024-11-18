// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "TextSprite.h"
#include "sdlUtils.h"
#include <algorithm>

using namespace agp;

TextSprite::TextSprite(
	const std::string& text,
	const std::string& fontPath,
	const Color& fontColor,
	const PointF& margin,
	const PointF& maxSize,
	VAlign vertAlign,
	HAlign horzAlign,
	Style style)
	: Sprite(nullptr)
{
	_text = text;
	_fontPath = fontPath;
	_fontColor = fontColor;
	_margin = margin;
	_maxSize = maxSize;
	_fontStyle = style;
	_vertAlign = vertAlign;
	_horzAlign = horzAlign;
	_regenerateTexture = true;
}

TextSprite::~TextSprite()
{
	if(_spritesheet)
		SDL_DestroyTexture(_spritesheet);
}

void TextSprite::setText(const std::string& newText)
{ 
	if(_text != newText)
		_regenerateTexture = true;
	_text = newText; 
}

void TextSprite::setColor(const Color& newColor)
{ 
	if(_fontColor != newColor)
		_regenerateTexture = true;
	_fontColor = newColor; 
}

void TextSprite::setMargin(const PointF& newMargin)
{
	_margin = newMargin;
}

void TextSprite::render(
	SDL_Renderer* renderer,
	const RectF& drawRect,
	Transform camera,
	const Point& pixelUnitSize,
	float angle,
	SDL_RendererFlip flip)
{
#ifdef WITH_TTF
	// re-generate texture only when needed (text/color change)
	// since it is an expensive operation
	if (_regenerateTexture)
	{
		if (_spritesheet)
			SDL_DestroyTexture(_spritesheet);
		_spritesheet = generateText(_text, renderer, _fontPath, _fontColor, pointSize, int(_fontStyle));
		SDL_QueryTexture(_spritesheet, nullptr, nullptr, &_rect.size.x, &_rect.size.y);
		_regenerateTexture = false;
	}

	// apply margin
	RectF correctedDrawRect = drawRect;
	correctedDrawRect.pos.x += _margin.x;
	correctedDrawRect.pos.y += _margin.y;
	correctedDrawRect.size.x -= 2 * _margin.x;
	correctedDrawRect.size.y -= 2 * _margin.y;

	// get the original texture size
	float originalWidth = static_cast<float>(_rect.size.x);
	float originalHeight = static_cast<float>(_rect.size.y);

	// calculate scale factors
	float scaleX = correctedDrawRect.size.x / originalWidth;
	float scaleY = correctedDrawRect.size.y / originalHeight;

	// choose the smaller scale factor to maintain aspect ratio
	float scale = (std::min)(scaleX, scaleY);

	// calculate the initial scaled size
	float scaledWidth = originalWidth * scale;
	float scaledHeight = originalHeight * scale;

	// apply maximum width and height constraints to the scaled size
	if (_maxSize.x && scaledWidth > _maxSize.x)
	{
		scale = _maxSize.x / originalWidth;
		scaledWidth = _maxSize.x;
		scaledHeight = originalHeight * scale;
	}

	if (_maxSize.y && scaledHeight > _maxSize.y)
	{
		scale = _maxSize.y / originalHeight;
		scaledHeight = _maxSize.y;
		scaledWidth = originalWidth * scale;
	}

	// initialize correctedDrawRectAR
	RectF correctedDrawRectAR = correctedDrawRect;
	correctedDrawRectAR.size.x = scaledWidth;
	correctedDrawRectAR.size.y = scaledHeight;

	// horizontal alignment
	if (_horzAlign == HAlign::CENTER)
		correctedDrawRectAR.pos.x = correctedDrawRect.pos.x + (correctedDrawRect.size.x - correctedDrawRectAR.size.x) / 2;
	else if (_horzAlign == HAlign::RIGHT)
		correctedDrawRectAR.pos.x = correctedDrawRect.pos.x + correctedDrawRect.size.x - correctedDrawRectAR.size.x;
	else // HAlign::LEFT
		correctedDrawRectAR.pos.x = correctedDrawRect.pos.x;

	// vertical alignment
	if (_vertAlign == VAlign::CENTER)
		correctedDrawRectAR.pos.y = correctedDrawRect.pos.y + (correctedDrawRect.size.y - correctedDrawRectAR.size.y) / 2;
	else if ( (_vertAlign == VAlign::BOTTOM && !drawRect.yUp) || (_vertAlign == VAlign::TOP && drawRect.yUp))
		correctedDrawRectAR.pos.y = correctedDrawRect.pos.y + correctedDrawRect.size.y - correctedDrawRectAR.size.y;
	else // VAlign::TOP && !drawRect.yUp || VAlign::BOTTOM && drawRect.yUp 
		correctedDrawRectAR.pos.y = correctedDrawRect.pos.y;

	SDL_Rect srcRect = _rect.toSDL();
	SDL_FRect drawRect_sdl = RectF(camera(correctedDrawRectAR.tl()), camera(correctedDrawRectAR.br())).toSDLf();

	SDL_RenderCopyF(renderer, _spritesheet, &srcRect, &drawRect_sdl);
#else
	if (_regenerateTexture)
	{
		printf("%s\n", _text.c_str());
		_regenerateTexture = false;
	}
#endif
}