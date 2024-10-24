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
	class AnimatedSprite;
}

// AnimatedSprite
// - implements animations
class agp::AnimatedSprite : public Sprite
{
	protected:

		std::vector<RectI> _frames;
		float _frameIterator;
		float _FPS;

	public:

		AnimatedSprite(SDL_Texture* spritesheet, const std::vector<RectI>& frames, float FPS);

		void setFPS(float fps) { _FPS = fps; }

		// extends update method (+animations)
		virtual void update(float dt) override;
};