// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "AnimatedSprite.h"

using namespace agp;

AnimatedSprite::AnimatedSprite(SDL_Texture* spritesheet, const std::vector<RectI>& frames, float FPS) 
	: Sprite(spritesheet, frames[0])
{
	_frames = frames;
	_FPS = FPS;
	_frameIterator = 0;
}

void AnimatedSprite::update(float dt)
{
	Sprite::update(dt);

	// update current frame based on frame rate and delta time
	_frameIterator += _FPS * dt;

	// wrap current frame if needed
	while (_frameIterator >= _frames.size())
		_frameIterator -= _frames.size();

	// set current spritesheet rect
	_rect = _frames[static_cast<int>(_frameIterator)];
}