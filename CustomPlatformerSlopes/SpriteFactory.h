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
#include <SDL3/SDL.h>
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
		std::map<std::string, SDL_Texture*> _levelBackgrounds;

		// constructor accessible only to Singleton (thanks to friend declaration)
		SpriteFactory();

	public:

		// creation
		Sprite* get(const std::string& id);
		Sprite* getNumber(int number, const Vec2Df& size = { 1,1 }, int fillN = 0, char fillChar = '0');
		Sprite* getText(std::string text, const Vec2Df& size = { 1,1 }, int fillN = 0, char fillChar = ' ');
		Sprite* loadLevelBackground(const std::string& fileName);
};