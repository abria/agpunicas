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
static RectI link_down(1, 1, 16, 24);
static RectI link_up(1, 101, 16, 24);
static RectI link_right(1, 51, 16, 24);
static RectI hud_letter(519, 254, 8, 8);
static RectI hud_number(519, 263, 8, 8);
static RectI hud_letter_disabled(519, 366, 8, 8);
static RectI hud_number_disabled(519, 375, 8, 8);
static RectI hud_coin(519, 289, 8, 8);
static RectI hudzelda_number(259, 13, 7, 7);
static RectI hudzelda_heart(259, 56, 7, 7);

void SpriteFactory::init(SDL_Renderer* renderer)
{
	_renderer = renderer;
	_spriteSheets["overworld"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "/sprites/scene_overworld.png");
	_spriteSheets["linkhouse"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/scene_linkhouse.png");
	_spriteSheets["welcome"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/welcome.png");
	_spriteSheets["hud_mario"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/hud_mario.png", { 147, 187, 236 });
	_spriteSheets["hud"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/hud.png", { 255, 0, 255 });
	_spriteSheets["inventory"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/menus3.png", { 0, 91, 127 });
	_spriteSheets["player"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/player_corrected.png", _autoTiles["player"], { 0, 128, 128 }, {0, 64, 64});
	_spriteSheets["enemies"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/enemies_3.png", _autoTiles["enemies"], { 255, 0, 255 }, { 128, 255, 255 });
	_spriteSheets["fonts"] = loadTextureConnectedComponents(renderer, std::string(SDL_GetBasePath()) + "sprites/fonts.png", _autoComponents["fonts"], {56, 56, 94});
	_spriteSheets["npcs_other_worlds"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/npcs_other_worlds.png", { 255, 0, 255 });

	//_spriteSheets["knight"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/knight.png", _autoTiles["knight"], { 0, 128, 128 }, { 0, 255, 0 }, 5, false, false, true);


	// font rectangles manual correction
	for (int i = 0; i < _autoComponents["fonts"].size(); i++)
	{
		if (i < 26)
			_autoComponents["fonts"][i].adjust(0, 0, 0, 2);
		else if (i == 27)
			_autoComponents["fonts"][i].adjust(0, -9, 0, 2);
		else if (i >= 39 && i <= 40)
		{
			_autoComponents["fonts"][i].pos.y = 173;
			_autoComponents["fonts"][i].size.y = 15;
		}
		else if (i >= 44 && i <= 45)
			_autoComponents["fonts"][i].adjust(0, 0, 0, 2);
		else if (i >= 48 && i <= 57)
		{
			_autoComponents["fonts"][i].pos.y = 205;
			_autoComponents["fonts"][i].size.y = 15;
		}
		else if (i >= 58 && i <= 67)
		{
			_autoComponents["fonts"][i].pos.y = 221;
			_autoComponents["fonts"][i].size.y = 15;
		}
		else if (i >= 68)
		{
			_autoComponents["fonts"][i].pos.y = 237;
			_autoComponents["fonts"][i].size.y = 15;
		}
	}

	// sprite creation functions (see get)

	// single-frame sprites
	_creators["welcome"] = [this] { return new Sprite(_spriteSheets["welcome"]); };
	_creators["hud"] = [this] { return new Sprite(_spriteSheets["hud"], RectI(0, 0, 16 * 16, 16 * 14)); };
	_creators["inventory"] = [this] { return new Sprite(_spriteSheets["inventory"], RectI(8, 10, 16 * 16, 16 * 14)); };
	_creators["gameover"] = [this] { return new Sprite(_spriteSheets["hud_mario"], RectI(260, 253, 16 * 16, 15 * 16)); };
	_creators["overworld"] = [this] { return new Sprite(_spriteSheets["overworld"]); };
	_creators["linkhouse"] = [this] { return new Sprite(_spriteSheets["linkhouse"]); };
	_creators["link_stand_DOWN"] = [this] { return new Sprite(_spriteSheets["player"], _autoTiles["player"][0][0]); };
	_creators["link_stand_UP"] = [this] { return new Sprite(_spriteSheets["player"], _autoTiles["player"][4][0]); };
	_creators["link_stand_RIGHT"] = [this] { return new Sprite(_spriteSheets["player"], _autoTiles["player"][2][0]); };
	_creators["link_stand_LEFT"] = [this] { return new Sprite(_spriteSheets["player"], _autoTiles["player"][2][0]); };
	_creators["link_shadow"] = [this] { return new Sprite(_spriteSheets["player"], _autoTiles["player"][6].back()); };
	_creators["npc_example"] = [this] { return new Sprite(_spriteSheets["npcs_other_worlds"], RectI(556, 67, 20, 26)); };

	// inventory (index = item category, e.g. "inventory_icon_3")
	_indexedCreators["inventory_icon_"] = [this](int index)
	{
		int y = index / 5;
		int x = index % 5;
		return new Sprite(_spriteSheets["inventory"], moveBy(RectI(116, 560, 16, 16), x, y, 16, 16, 56, 56));
	};
	_indexedCreators["inventory_label_"] = [this](int index)
	{
		int y = index / 5;
		int x = index % 5;
		return new Sprite(_spriteSheets["inventory"], moveBy(RectI(92, 608, 64, 16), x, y, 64, 16, 8, 56));
	};
	_creators["inventory_selected"] = [this] { return new Sprite(_spriteSheets["inventory"], RectI(464, 328, 32, 32)); };

	// hud
	_creators["hud_heart"] = [this] { return new Sprite(_spriteSheets["hud"], hudzelda_heart); };
	_creators["hud_heart_half"] = [this] { return new Sprite(_spriteSheets["hud"], moveBy(hudzelda_heart, 2, 0, 7, 7)); };
	_creators["hud_heart_empty"] = [this] { return new Sprite(_spriteSheets["hud"], moveBy(hudzelda_heart, 4, 0, 7, 7)); };

	// animated sprites
	_creators["link_walk_DOWN"] = [this] { return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][0].begin() + 1, _autoTiles["player"][0].begin() + 8 }, 20); };
	_creators["link_attack_DOWN"] = [this] { return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][1].begin(), _autoTiles["player"][1].begin() + 6 }, 44, { 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5 }, 1); };
	_creators["link_walk_UP"] = [this] { return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][4].begin() + 1, _autoTiles["player"][4].begin() + 8 }, 20); };
	_creators["link_attack_UP"] = [this] { return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][5].begin(), _autoTiles["player"][5].begin() + 6 }, 44, {0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5}, 1); };
	_creators["link_walk_RIGHT"] = _creators["link_walk_LEFT"] = [this] { return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][2].begin() + 1, _autoTiles["player"][2].begin() + 8 }, 20); };
	_creators["link_attack_RIGHT"] = _creators["link_attack_LEFT"] = [this] { return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][3].begin(), _autoTiles["player"][3].begin() + 6 }, 44, { 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5 }, 1); };
	_creators["link_sword_RIGHT"] = [this] { return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][9].begin(), _autoTiles["player"][9].begin() + 9 }, 44, {0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 8}, 1); };
	_creators["soldier_attack_DOWN"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][0].begin(), _autoTiles["enemies"][0].begin() + 4 }, 8); };
	_creators["soldier_attack_RIGHT"] = _creators["soldier_attack_LEFT"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][2].begin(), _autoTiles["enemies"][2].begin() + 3 }, 6); };
	_creators["soldier_attack_UP"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][4].begin(), _autoTiles["enemies"][4].begin() + 4 }, 8); };
	_creators["soldier_DOWN"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][1].begin(), _autoTiles["enemies"][1].begin() + 3 }, 6); };
	_creators["soldier_RIGHT"] = _creators["soldier_LEFT"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][3].begin(), _autoTiles["enemies"][3].begin() + 3 }, 6); };
	_creators["soldier_UP"] = [this] { return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][5].begin(), _autoTiles["enemies"][5].begin() + 3 }, 6); };
	_creators["hud_coin"] = [this]
	{
		std::vector< RectI> rects;
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 2, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 0, 0));
		return new AnimatedSprite(_spriteSheets["hud_mario"], rects, 6);
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

Sprite* SpriteFactory::getChar(char c)
{
	if (c >= 'A' && c <= 'Z')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][c-'A']);
	else if (c >= 'a' && c <= 'z')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][48 + c-'a']);
	else if (c == '.')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][27]);
	else if (c == ',')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][28]);
	else if (c == '\'')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][28]);
	else if (c == '!')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][44]);
	else if (c == '?')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][45]);
	else if (c == '<')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][39]);
	else if (c == '>')
		return new Sprite(_spriteSheets["fonts"], _autoComponents["fonts"][40]);
	else
		return nullptr;
}

Sprite* SpriteFactory::getTextSMB(std::string text, const Vec2Df& size, int fillN, char fillChar, bool enabled)
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

	return new TiledSprite(_spriteSheets["hud_mario"], tiles, size);
}

Sprite* SpriteFactory::getNumberHUD(int n, int fill)
{
	std::vector< RectI> tiles;

	std::string text = std::to_string(n);

	if (fill)
		while (text.size() != fill)
			text = '0' + text;

	RectI& number_anchor = hudzelda_number;

	for (auto& c : text)
	{
		if (isdigit(c))
			tiles.push_back(moveBy(number_anchor, c - '0' - (c - '0' < 5 ? 0 : 5), c - '0' < 5 ? 0 : 1, 7, 7));
	}

	return new TiledSprite(_spriteSheets["hud"], tiles, {7, 7});
}