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
	_spriteSheets["overworld"] = loadTexture(renderer, "sprites/scene_overworld.png");
	_spriteSheets["welcome"] = loadTexture(renderer, "sprites/welcome.png");
	_spriteSheets["hud_mario"] = loadTexture(renderer, "sprites/hud_mario.png", { 147, 187, 236 });
	_spriteSheets["hud"] = loadTexture(renderer, "sprites/hud.png", { 255, 0, 255 });
	_spriteSheets["inventory"] = loadTexture(renderer, "sprites/menus3.png", { 0, 91, 127 });
	_spriteSheets["player"] = loadTextureAutoDetect(renderer, "sprites/player_corrected.png", _autoTiles["player"], { 0, 128, 128 }, {0, 64, 64});
	_spriteSheets["enemies"] = loadTextureAutoDetect(renderer, "sprites/enemies_3.png", _autoTiles["enemies"], { 255, 0, 255 }, { 128, 255, 255 });
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
	else if (id == "link_stand_DOWN")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][0][0], Direction::DOWN);
	else if (id == "link_stand_UP")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][4][0], Direction::UP);
	else if (id == "link_stand_RIGHT")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][2][0], Direction::RIGHT);
	else if (id == "link_stand_LEFT")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][2][0], Direction::LEFT);
	else if (id == "link_shadow")
		return new Sprite(_spriteSheets["player"], _autoTiles["player"][6].back(), Direction::UP);

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
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][0].begin() + 1, _autoTiles["player"][0].begin() + 8 }, 20, Direction::UP);
	else if (id == "link_attack_DOWN")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][1].begin(), _autoTiles["player"][1].begin() + 6 }, 44, Direction::UP, { 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5 }, 1);
	else if (id == "link_walk_UP")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][4].begin() + 1, _autoTiles["player"][4].begin() + 8 }, 20, Direction::UP);
	else if (id == "link_attack_UP")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][5].begin(), _autoTiles["player"][5].begin() + 6 }, 44, Direction::UP, {0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5}, 1);
	else if (id == "link_walk_RIGHT" || id == "link_walk_LEFT")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][2].begin() + 1, _autoTiles["player"][2].begin() + 8 }, 20, Direction::UP);
	else if (id == "link_attack_RIGHT" || id == "link_attack_LEFT")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][3].begin(), _autoTiles["player"][3].begin() + 6 }, 44, id == "link_attack_RIGHT" ? Direction::RIGHT : Direction::LEFT, { 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 5 }, 1);
	else if (id == "link_sword_RIGHT")
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][9].begin(), _autoTiles["player"][9].begin() + 9 }, 44, Direction::NONE, {0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 8}, 1);
	else if (id == "soldier_attack_DOWN")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][0].begin(), _autoTiles["enemies"][0].begin() + 4 }, 8, Direction::DOWN);
	else if (id == "soldier_attack_RIGHT" || id == "soldier_attack_LEFT")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][2].begin(), _autoTiles["enemies"][2].begin() + 3 }, 6, id == "soldier_attack_RIGHT" ? Direction::RIGHT : Direction::LEFT);
	else if (id == "soldier_attack_UP")
		return new AnimatedSprite(_spriteSheets["enemies"], { _autoTiles["enemies"][4].begin(), _autoTiles["enemies"][4].begin() + 4 }, 8, Direction::UP);
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