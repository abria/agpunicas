// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteFactory.h"
#include <SDL3_image/SDL_image.h>
#include "graphicsUtils.h"
#include "sdlUtils.h"
#include "AnimatedSprite.h"
#include "TiledSprite.h"
#include "FilledSprite.h"
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace agp;

// anchors
static RectI hud_letter(519, 254, 8, 8);
static RectI hud_number(519, 263, 8, 8);
static RectI hud_letter_disabled(519, 366, 8, 8);
static RectI hud_number_disabled(519, 375, 8, 8);
static RectI hud_coin(519, 289, 8, 8);

void SpriteFactory::init(SDL_Renderer* renderer)
{
	_renderer = renderer;
	_spriteSheets["mario"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/mario.png", _autoTiles["mario"], {27, 89, 153}, { 147, 187, 236 });
	_spriteSheets["enemies"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/enemies.png", _autoTiles["enemies"], { 27, 89, 153 }, { 147, 187, 236 }, 17);
	_spriteSheets["hud"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/hud.png", { 147, 187, 236 });
	_spriteSheets["tiles"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/stage_tiles.png", _autoTiles["tiles"], { 27, 89, 153 }, { 147, 187, 236 }, 5, true, false);
	_spriteSheets["link"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/link.png", _autoTiles["link"], { 0, 128, 128 }, { 0, 64, 64 });
	_spriteSheets["marco"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/marco.png", _autoTiles["marco"], { 255, 0, 255 }, { 0, 255, 0 });

	// sprite creation functions (see get)

	// single-frame sprites
	_creators["terrain"] = [this] { return new FilledSprite(_spriteSheets["tiles"], _autoTiles["tiles"][7][0]); };
	_creators["brick"] = [this] { return new Sprite(_spriteSheets["tiles"], _autoTiles["tiles"][5][1]); };
	_creators["wall"] = [this] { return new Sprite(_spriteSheets["tiles"], _autoTiles["tiles"][5][2]); };
	_creators["block"] = [this] { return new Sprite(_spriteSheets["tiles"], _autoTiles["tiles"][3][5]); };
	_creators["welcome"] = [this] { return new Sprite(_spriteSheets["hud"], RectI(1, 2 + 16 * 2, 16 * 16, 13 * 16)); };
	_creators["gameover"] = [this] { return new Sprite(_spriteSheets["hud"], RectI(260, 253, 16 * 16, 15 * 16)); };
	_creators["mario_stand"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][0]); };
	_creators["mario_jump"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][6]); };
	_creators["mario_skid"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][5]); };
	_creators["mario_die"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][1]); };
	_creators["mario_attack"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][4]); };
	_creators["hammer"] = [this] { return new Sprite(_spriteSheets["enemies"], _autoTiles["enemies"][0][25]); };
	_creators["hammer_brother_jump"] = [this] { return new Sprite(_spriteSheets["enemies"], _autoTiles["enemies"][0][24]); };
	_creators["platform"] = [this] { return new FilledSprite(_spriteSheets["tiles"], _autoTiles["tiles"][8][33]); };

	// animated sprites
	_creators["mario_walk"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin() + 2, _autoTiles["mario"][0].begin() + 5 }, 10); };
	_creators["mario_run"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin() + 2, _autoTiles["mario"][0].begin() + 5 }, 20); };
	_creators["box"] = [this] { return new AnimatedSprite(_spriteSheets["tiles"], { _autoTiles["tiles"][3].begin(), _autoTiles["tiles"][3].begin() + 3 }, 5, {0, 1, 2, 1, 0}); };
	_creators["hud_coin"] = [this]
	{
		std::vector< RectI> rects;
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 2, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 0, 0));
		return new AnimatedSprite(_spriteSheets["hud"], rects, 6);
	};
	_creators["hammer_brother_walk"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][0].begin() + 21, _autoTiles["enemies"][0].begin() + 23 }, 8); };
	_creators["hammer_brother_throw"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][0].begin() + 23, _autoTiles["enemies"][0].begin() + 25 }, 8); };
	_creators["link_sword"] = [this] { return new AnimatedSprite(_spriteSheets["link"], { _autoTiles["link"][9].begin(), _autoTiles["link"][9].begin() + 9 }, 22, { 0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 8 }, 1); };
	_creators["marco_stand"] = [this] { return new AnimatedSprite(_spriteSheets["marco"], { _autoTiles["marco"][0].begin(), _autoTiles["marco"][0].begin() + 5 }, 10); };
	_creators["marco_walk"] = [this] { return new AnimatedSprite(_spriteSheets["marco"], { _autoTiles["marco"][1].begin(), _autoTiles["marco"][1].begin() + 14 }, 10); };
	_creators["marco_jump"] = [this] { return new AnimatedSprite(_spriteSheets["marco"], { _autoTiles["marco"][2].begin(), _autoTiles["marco"][2].begin() + 12 }, 10); };
	_creators["marco_attack"] = [this] { return new AnimatedSprite(_spriteSheets["marco"], { _autoTiles["marco"][3].begin(), _autoTiles["marco"][3].begin() + 9 }, 30); };

	// tiled sprites (index = pipe length, e.g. "pipe3")
	_indexedCreators["pipe"] = [this](int length)
	{
		std::vector< RectI> rects;
		rects.push_back(_autoTiles["tiles"][2][6]);
		rects.push_back(_autoTiles["tiles"][2][7]);
		for (int i = 0; i < length; i++)
		{
			rects.push_back(_autoTiles["tiles"][3][6]);
			rects.push_back(_autoTiles["tiles"][3][7]);
		}
		return new TiledSprite(_spriteSheets["tiles"], rects);
	};
}

SpriteFactory::~SpriteFactory()
{
	for (auto& entry : _spriteSheets)
		SDL_DestroyTexture(entry.second);
}

Sprite* SpriteFactory::get(const std::string& id)
{
	// sprite with a fixed id, e.g. "brick"
	auto creator = _creators.find(id);
	if (creator != _creators.end())
		return creator->second();

	// indexed sprite: prefix followed by a non-negative integer, e.g. "pipe12"
	for (auto& entry : _indexedCreators)
	{
		const std::string& prefix = entry.first;
		if (id.size() > prefix.size() && id.size() - prefix.size() < 10 && id.compare(0, prefix.size(), prefix) == 0)
		{
			std::string index = id.substr(prefix.size());
			if (std::all_of(index.begin(), index.end(), [](unsigned char c) { return std::isdigit(c) != 0; }))
				return entry.second(std::stoi(index));
		}
	}

	std::cerr << "Cannot find sprite \"" << id << "\"\n";
	return nullptr;
}

Sprite* SpriteFactory::getText(std::string text, const Vec2Df& size, int fillN, char fillChar, bool enabled)
{
	std::vector< RectI> tiles;

	if (fillN)
		while (text.size() != fillN)
			text = fillChar + text;

	RectI& number_anchor = enabled ? hud_number : hud_number_disabled;
	RectI& letter_anchor = enabled ? hud_letter : hud_letter_disabled;

	for (auto& c : text)
	{
		if(isdigit(c))
			tiles.push_back(moveBy(number_anchor, c - '0', 0, 8, 8));
		else if (isalpha(c))
			tiles.push_back(moveBy(letter_anchor, toupper(c) - 'A', 0, 8, 8));
		else if (c == '-')
			tiles.push_back(moveBy(number_anchor, 10, 0, 8, 8));
		else if (c == '*')
			tiles.push_back(moveBy(number_anchor, 11, 0, 8, 8));
		else if (c == '!')
			tiles.push_back(moveBy(number_anchor, 12, 0, 8, 8));
		else if (c == '\xA9')
			tiles.push_back(moveBy(number_anchor, 13, 0, 8, 8));
		else if (c == '.')
			tiles.push_back(moveBy(number_anchor, 14, 0, 8, 8));
		else
			tiles.push_back(moveBy(hud_letter, 0, -5, 8, 8));	// empty space
	}

	return new TiledSprite(_spriteSheets["hud"], tiles, size);
}