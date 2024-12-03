// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteFactory.h"
#include "SDL_image.h"
#include "graphicsUtils.h"
#include "sdlUtils.h"
#include "Window.h"
#include "AnimatedSprite.h"
#include "TiledSprite.h"
#include "FilledSprite.h"
#include "Game.h"
#include <iostream>

using namespace agp;

SpriteFactory::SpriteFactory()
{
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
		return;
	}

	SDL_Renderer* renderer = Game::instance()->window()->renderer();
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
}

SpriteFactory::~SpriteFactory()
{
	// todo: deallocate all assets
}

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

Sprite* SpriteFactory::get(const std::string& id)
{
	std::vector< RectI> rects;

	// single-frame sprites
	if (id == "welcome")
		return new Sprite(_spriteSheets["welcome"]);
	else if (id == "hud")
		return new Sprite(_spriteSheets["hud"], RectI(0, 0, 16 * 16, 16 * 14));
	else if (id == "inventory")
		return new Sprite(_spriteSheets["inventory"], RectI(8, 10, 16 * 16, 16 * 14));
	else if (id == "gameover")
		return new Sprite(_spriteSheets["hud_mario"], RectI(260, 253, 16 * 16, 15 * 16));
	else if (id == "overworld")
		return new Sprite(_spriteSheets["overworld"]);
	else if (id == "linkhouse")
		return new Sprite(_spriteSheets["linkhouse"]);
	else if (id == "link_stand_DOWN")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][0][0]);
	else if (id == "link_stand_UP")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][4][0]);
	else if (id == "link_stand_RIGHT")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][2][0]);
	else if (id == "link_stand_LEFT")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][2][0]);
	else if (id == "link_shadow")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][6].back());
	else if (id == "npc_example")
		return new Sprite(_spriteSheets["npcs_other_worlds"], RectI(556, 67, 20, 26));

	// inventory
	else if (id.compare(0, 15, "inventory_icon_") == 0)
	{
		std::string digits = id.substr(15);
		int index = std::stoi(digits);
		int y = index / 5;
		int x = index % 5;
		return new Sprite(_spriteSheets["inventory"], moveBy(RectI(116, 560, 16, 16), x, y, 16, 16, 56, 56));
	}
	else if (id.compare(0, 16, "inventory_label_") == 0)
	{
		std::string digits = id.substr(16);
		int index = std::stoi(digits);
		int y = index / 5;
		int x = index % 5;
		return new Sprite(_spriteSheets["inventory"], moveBy(RectI(92, 608, 64, 16), x, y, 64, 16, 8, 56));
	}
	else if (id == "inventory_selected")
		return new Sprite(_spriteSheets["inventory"], RectI(464, 328, 32, 32));

	// hud
	else if (id == "hud_heart")
		return new Sprite(_spriteSheets["hud"], hudzelda_heart);
	else if (id == "hud_heart_half")
		return new Sprite(_spriteSheets["hud"], moveBy(hudzelda_heart, 2, 0, 7, 7));
	else if (id == "hud_heart_empty")
		return new Sprite(_spriteSheets["hud"], moveBy(hudzelda_heart, 4, 0, 7, 7));

	// animated sprites
	else if (id == "link_walk_DOWN")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][0].begin() + 1, _autoTiles["player"][0].begin() + 8 }, 20);
	else if (id == "link_attack_DOWN")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][1].begin(), _autoTiles["player"][1].begin() + 6 }, 44, { 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5 }, 1);
	else if (id == "link_walk_UP")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][4].begin() + 1, _autoTiles["player"][4].begin() + 8 }, 20);
	else if (id == "link_attack_UP")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][5].begin(), _autoTiles["player"][5].begin() + 6 }, 44, {0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5}, 1);
	else if (id == "link_walk_RIGHT" || id == "link_walk_LEFT")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][2].begin() + 1, _autoTiles["player"][2].begin() + 8 }, 20);
	else if (id == "link_attack_RIGHT" || id == "link_attack_LEFT")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][3].begin(), _autoTiles["player"][3].begin() + 6 }, 44, { 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5 }, 1);
	else if (id == "link_sword_RIGHT")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][9].begin(), _autoTiles["player"][9].begin() + 9 }, 44, {0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 8}, 1);
	else if (id == "soldier_attack_DOWN")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][0].begin(), _autoTiles["enemies"][0].begin() + 4 }, 8);
	else if (id == "soldier_attack_RIGHT" || id == "soldier_attack_LEFT")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][2].begin(), _autoTiles["enemies"][2].begin() + 3 }, 6);
	else if (id == "soldier_attack_UP")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][4].begin(), _autoTiles["enemies"][4].begin() + 4 }, 8);
	else if (id == "soldier_DOWN")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][1].begin(), _autoTiles["enemies"][1].begin() + 3 }, 6);
	else if (id == "soldier_RIGHT" || id == "soldier_LEFT")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][3].begin(), _autoTiles["enemies"][3].begin() + 3 }, 6);
	else if (id == "soldier_UP")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][5].begin(), _autoTiles["enemies"][5].begin() + 3 }, 6); 
	else if (id == "hud_coin")
	{
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 2, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 0, 0));
		return new AnimatedSprite(_spriteSheets["hud_mario"], rects, 6);
	}

	// tiled sprites

	else
	{
		std::cerr << "Cannot find sprite \"" << id << "\"\n";
		return nullptr;
	}
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
		else if (c == '©')
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