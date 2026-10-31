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

void SpriteFactory::init(SDL_Renderer* renderer)
{
	_renderer = renderer;

	_spriteSheets["mario"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/mario.png", _autoTiles["mario"], { 41, 88, 124 }, { 68, 145, 190 });
	_spriteSheets["titlescreen"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/titlescreen.png", {224, 163, 216});
	_spriteSheets["font"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/font.png", { 94, 219, 146 });
	_spriteSheets["hud"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/hud.png");
	_spriteSheets["stages"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/stages.png", { 224, 163, 216 });
	_spriteSheets["items"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/items.png", _autoTiles["items"], { 41, 88, 124 }, { 68, 145, 190 });
	_spriteSheets["enemies"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/enemies.png", { 68, 145, 190 });

	// sprite creation functions (see get)
	_creators["level_1_1"] = [this] { return loadLevelBackground("1-1.png"); };

	// screens
	_creators["welcome"] = [this] { return new Sprite(_spriteSheets["titlescreen"], RectI(783, 244, 16 * 16, 16 * 14)); };
	_creators["flashing_3"] = [this]
	{
		std::vector< RectI> rects;
		RectI welcome3(115, 574, 32, 48);
		rects.push_back(welcome3);
		rects.push_back(welcome3 + Point(1 * 260, 0));
		rects.push_back(welcome3 + Point(2 * 260, 0));
		rects.push_back(welcome3 + Point(3 * 260, 0));
		return new AnimatedSprite(_spriteSheets["titlescreen"], rects, 10, {0, 1, 0, 2, 3, 2, 0});
	};

	// points (index = points digit, e.g. "points_3")
	_indexedCreators["points_"] = [this](int point_idx) -> Sprite*
	{
		if (point_idx < 4)
			return new Sprite(_spriteSheets["font"], moveBy(RectI(60, 89, 12, 8), point_idx, 0, 12, 8, 3));
		else if (point_idx < 9)
			return new Sprite(_spriteSheets["font"], moveBy(RectI(119, 89, 16, 8), point_idx-4, 0, 16, 8, 3));
		else
			return nullptr;
	};

	// HUD
	_creators["hud"] = [this] { return new Sprite(_spriteSheets["hud"], RectI(4, 81, 16 * 16, 16 * 3)); };
	_creators["hud_arrow"] = [this] { return new Sprite(_spriteSheets["hud"], RectI(625, 25, 8, 8)); };
	_creators["hud_P"] = [this] { return new Sprite(_spriteSheets["hud"], RectI(653, 25, 16, 8)); };

	// Wooden moving platform.
	_creators["platform"] = [this] { return new FilledSprite(_spriteSheets["stages"], RectI(104, 199, 16, 16), {1, 0.5f}); };

	// Mario
	_creators["mario_stand"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][0]); };
	_creators["mario_jump"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][2]); };
	_creators["mario_jumpP"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][5]); };
	_creators["mario_fall"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][2]); };
	_creators["mario_skid"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][6]); };
	_creators["mario_die"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][17]); };
	_creators["mario_walk"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin(), _autoTiles["mario"][0].begin() + 2 }, 10); };
	_creators["mario_run"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin(), _autoTiles["mario"][0].begin() + 2 }, 30); };
	_creators["mario_pspeed"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin() + 3, _autoTiles["mario"][0].begin() + 5 }, 30); };
	_creators["mario_small2big"] = [this]
	{
		std::vector< RectI> rects;
		rects.push_back(_autoTiles["mario"][3][8]);
		rects.push_back(_autoTiles["mario"][0][0]);
		rects.push_back(_autoTiles["mario"][2][0]);
		return new AnimatedSprite(_spriteSheets["mario"], rects, 10, { 0, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 2 }, 1);
	};
	_creators["supermario_big2small"] = [this]
	{
		std::vector< RectI> rects;
		rects.push_back(_autoTiles["mario"][3][8]);
		rects.push_back(_autoTiles["mario"][0][0]);
		rects.push_back(_autoTiles["mario"][2][0]);
		return new AnimatedSprite(_spriteSheets["mario"], rects, 10, { 2, 0, 2, 0, 2, 0, 1, 0, 1, 0, 1, 0, 1 }, 1);
	};

	// Super Mario
	_creators["supermario_stand"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][0]); };
	_creators["supermario_jump"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][4]); };
	_creators["supermario_jumpP"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][8]); };
	_creators["supermario_fall"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][2]); };
	_creators["supermario_crouch"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][3]); };
	_creators["supermario_skid"] = [this] { return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][9]); };
	_creators["supermario_walk"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][2].begin(), _autoTiles["mario"][2].begin() + 3 }, 10, { 0, 1, 2, 1 }); };
	_creators["supermario_run"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][2].begin(), _autoTiles["mario"][2].begin() + 3 }, 30, { 0, 1, 2, 1 }); };
	_creators["supermario_pspeed"] = [this] { return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][2].begin() + 5, _autoTiles["mario"][2].begin() + 8 }, 30, { 0, 1, 2, 1 }); };

	// enemies
	_creators["goomba_walk"] = [this]
	{
		std::vector< RectI> rects;
		RectI anchor(1, 1, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, 1, 0, 16, 16, 1, 1));
		return new AnimatedSprite(_spriteSheets["enemies"], rects, 8);
	};
	_creators["goomba_stomped"] = [this] { return new Sprite(_spriteSheets["enemies"], RectI(35, 1, 16, 16)); };

	// level objects
	_creators["block_question"] = [this]
	{
		std::vector< RectI> rects;
		RectI anchor(104, 235, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, 1, 0, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, -1, 1, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 0, 1, 16, 16, 2, 2));
		return new AnimatedSprite(_spriteSheets["stages"], rects, 8);
	};
	_creators["block_empty"] = [this] { return new Sprite(_spriteSheets["stages"], RectI(122, 199, 16, 16)); };
	_creators["brick"] = [this]
	{
		std::vector< RectI> rects;
		RectI anchor(122, 253, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, -2, 1, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, -1, 1, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 0, 1, 16, 16, 2, 2));
		return new AnimatedSprite(_spriteSheets["stages"], rects, 8);
	};
	_creators["brick_debris"] = [this] { return new AnimatedSprite(_spriteSheets["items"], { _autoTiles["items"][4].begin() + 4, _autoTiles["items"][4].begin() + 6 }, 10); };
	_creators["coin"] = [this]
	{
		std::vector< RectI> rects;
		RectI anchor(86, 217, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, 1, 0, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 2, 0, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 0, 1, 16, 16, 2, 2));
		return new AnimatedSprite(_spriteSheets["stages"], rects, 8);
	};
	_creators["wood"] = [this] { return new Sprite(_spriteSheets["stages"], RectI(104, 199, 16, 16)); };
	_creators["spawnable_coin"] = [this] { return new AnimatedSprite(_spriteSheets["items"], { _autoTiles["items"][0].begin() + 19, _autoTiles["items"][0].begin() + 22 }, 16, { 0, 1, 2, 1 }); };
	_creators["mushroom_red"] = [this] { return new Sprite(_spriteSheets["items"], _autoTiles["items"][0][4]); };
	_creators["leaf"] = [this] { return new Sprite(_spriteSheets["items"], _autoTiles["items"][0][5]); };
	_creators["mushroom_green"] = [this] { return new Sprite(_spriteSheets["items"], _autoTiles["items"][0][7]); };
}

SpriteFactory::~SpriteFactory()
{
	for (auto& entry : _spriteSheets)
		SDL_DestroyTexture(entry.second);
	for (auto& entry : _levelBackgrounds)
		SDL_DestroyTexture(entry.second);
}

Sprite* SpriteFactory::loadLevelBackground(const std::string& fileName)
{
	if (_levelBackgrounds.count(fileName))
		return new Sprite(_levelBackgrounds.at(fileName));
	SDL_Texture* levelTexture = loadTexture(_renderer, std::string(SDL_GetBasePath()) + "sprites/levels/" + fileName);
	
	if (levelTexture)
	{
		_levelBackgrounds[fileName] = levelTexture;
		return new Sprite(levelTexture);
	}
	else
	{
		std::cerr << "Cannot find level background file \"" << fileName << "\"\n";
		return nullptr;
	}
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

Sprite* SpriteFactory::getText(std::string text, const Vec2Df& size, int fillN, char fillChar)
{
	std::vector< RectI> tiles;

	if (fillN)
		while (text.size() != fillN)
			text = fillChar + text;

	RectI row1_anchor(77, 31, 8, 8);
	RectI row2_anchor(77, 41, 8, 8);
	RectI row3_anchor(77, 51, 8, 8);

	for (auto& c : text)
	{
		if(isdigit(c))
			tiles.push_back(moveBy(row1_anchor, c - '0', 0, 8, 8, 2));
		else if (isalpha(c) && toupper(c) - 'A' < 2)
			tiles.push_back(moveBy(row1_anchor, toupper(c) - 'A' + 10, 0, 8, 8, 2));
		else if (isalpha(c) && toupper(c) - 'A' < 14)
			tiles.push_back(moveBy(row2_anchor, toupper(c) - 'A' - 2, 0, 8, 8, 2));
		else if (isalpha(c) && toupper(c) - 'A' > 13)
			tiles.push_back(moveBy(row3_anchor, toupper(c) - 'A' - 14, 0, 8, 8, 2));
		else
			tiles.push_back(RectI(196, 52, 8, 8));	// empty space
	}

	return new TiledSprite(_spriteSheets["font"], tiles, size);
}

Sprite* SpriteFactory::getNumber(int number, const Vec2Df& size, int fillN, char fillChar)
{
	std::vector< RectI> tiles;

	std::string str = std::to_string(number);

	if (fillN)
		while (str.size() != fillN)
			str = fillChar + str;

	RectI number_anchor_row1(524, 25, 8, 8);
	RectI number_anchor_row2(524, 34, 8, 8);

	for (auto& c : str)
	{
		if (isdigit(c) && c - '0' < 5)
			tiles.push_back(moveBy(number_anchor_row1, c - '0', 0, 8, 8));
		else if (isdigit(c) && c - '0' > 4)
			tiles.push_back(moveBy(number_anchor_row2, c - '0' - 5, 0, 8, 8));
		else
			tiles.push_back(moveBy(number_anchor_row2, 5, 0, 8, 8));	// empty space
	}

	return new TiledSprite(_spriteSheets["hud"], tiles, size);
}