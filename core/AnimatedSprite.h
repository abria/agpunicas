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
#include "mathUtils.h"

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
		int _loops;
		int _loopsStored;
		bool _paused;

	public:

		AnimatedSprite(
			SDL_Texture* spritesheet,
			const std::vector<RectI>& frames,
			float FPS,
			Direction keepARdir = Direction::NONE,
			const std::vector <int> resampling = std::vector<int>(),
			int loops = MAXINT);

		float FPS() const { return _FPS; }
		void setFPS(float fps) { _FPS = fps; }
		float duration() const { return _frames.size() / _FPS; }
		float currentTime() const { return _frameIterator / _frames.size(); }
		void setPaused(bool paused) { _paused = paused; }

		// extends update method (+animations)
		virtual void update(float dt) override;

		// extends reset method (+restart frameIterator )
		virtual void reset() override;
};