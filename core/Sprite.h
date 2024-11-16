// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "SDL.h"
#include "geometryUtils.h"

namespace agp
{
	class Sprite;
}

// Sprite
// - base class for sprites that blit texture data directly from spritesheets
class agp::Sprite
{
	protected:

		SDL_Texture* _spritesheet;		// spritesheet texture
		RectI _rect;					// in spritesheets coordinates
		Direction _expandDir;			// how to expand drawRect if its AR differs from _rect 

	public:

		Sprite(SDL_Texture* spritesheet, const RectI& rect = RectI(), Direction expandDir = Direction::NONE);
		virtual ~Sprite() {}
		RectI rect() { return _rect; }

		// render method (for rendering)
		virtual void render(
			SDL_Renderer* renderer, 
			const RectF& drawRect,		// drawable area in scene coords
			Transform camera,			// scene2view transform
			const Point& pixelUnitSize,	// scene unit size in pixels
			float angle = 0,			// rotation in degrees, clockwise
			SDL_RendererFlip flip = SDL_FLIP_NONE);

		// update method (for logic, animations)
		virtual void update(float dt) {};

		// reset method (for logic, animations)
		virtual void reset() {};
};