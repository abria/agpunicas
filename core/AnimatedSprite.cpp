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

AnimatedSprite::AnimatedSprite(
	SDL_Texture* spritesheet,
	const std::vector<RectI>& frames,
	float FPS,
	const std::vector <int> resampling,
	int loops)
	: Sprite(spritesheet, frames[0])
{
	if (resampling.size())
		for (auto& i : resampling)
			_frames.push_back(frames[i]);
	else 
		_frames = frames;

	_loops = loops;
	_loopsStored = loops;
	_FPS = FPS;
	_frameIterator = 0;
	_paused = false;
	_ended = false;
}

void AnimatedSprite::update(float dt)
{
	Sprite::update(dt);

	// animation stops if no loops are available or paused
	if (_loops <= 0 || _paused)
		return;

	// update current frame based on frame rate and delta time
	_frameIterator += _FPS * dt;

	// wrap current frame if needed
	while (_frameIterator >= _frames.size())
	{
		_frameIterator -= _frames.size();
		_loops--;
	}

	// animation ended: set last frame
	if (_loops <= 0)
	{
		_rect = _frames.back();
		_ended = true;
	}
	// set current spritesheet rect
	else
		_rect = _frames[static_cast<int>(_frameIterator)];
}

// extends reset method (+ restart frameIterator )
void AnimatedSprite::reset()
{
	_ended = false;
	_frameIterator = 0;
	_loops = _loopsStored;
}