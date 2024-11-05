// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <map>
#include <string>
#include "SDL.h"
#include "geometryUtils.h"
#include "Singleton.h"

namespace agp
{
	class Sprite;
	class SpriteFactory;
}

// SpriteFactory (singleton)
// - loads spritesheets
// - instances sprites by id
class agp::SpriteFactory : public Singleton<SpriteFactory>
{
	friend class Singleton<SpriteFactory>;

	private:

		std::map<std::string, SDL_Texture*> _spriteSheets;
		std::map<std::string, std::vector< std::vector<RectI > > > _autoTiles;

		// constructor inaccesible due to singleton
		SpriteFactory();

	public:

		// factory methods
		Sprite* get(const std::string& id);
		Sprite* getTextSMB(std::string text, const Vec2Df& size = { 1,1 }, int fillN = 0, char fillChar = ' ', bool enabled = true);
		Sprite* getNumberHUD(int n, int fill = 1);
};