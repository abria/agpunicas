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

SpriteFactory* SpriteFactory::instance()
{
	static SpriteFactory uniqueInstance;
	return &uniqueInstance;
}

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
	_spriteSheets["hud"] = loadTexture(renderer, "sprites/hud_mario.png", { 147, 187, 236 });
	_spriteSheets["player"] = loadTextureAutoDetect(renderer, "sprites/player_corrected.png", _autoTiles["player"], { 0, 128, 128 }, {0, 64, 64});
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

Sprite* SpriteFactory::get(const std::string& id)
{
	std::vector< RectI> rects;

	// single-frame sprites
	if (id == "welcome")
		return new Sprite(_spriteSheets["welcome"]);
	else if (id == "gameover")
		return new Sprite(_spriteSheets["hud"], RectI(260, 253, 16 * 16, 15 * 16));
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
		return new AnimatedSprite(_spriteSheets["player"], { _autoTiles["player"][9].begin(), _autoTiles["player"][9].begin() + 9 }, 44, Direction::NONE, {0, 1, 2, 3, 4, 5, 5, 6, 7, 8, 8});
	else if (id == "hud_coin")
	{
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 2, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 1, 0, 8, 8));
		rects.push_back(moveBy(hud_coin, 0, 0));
		rects.push_back(moveBy(hud_coin, 0, 0));
		return new AnimatedSprite(_spriteSheets["hud"], rects, 6);
	}

	// tiled sprites

	else
	{
		std::cerr << "Cannot find sprite \"" << id << "\"\n";
		return nullptr;
	}
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
		else if (c == '©')
			tiles.push_back(moveBy(number_anchor, 13, 0, 8, 8));
		else if (c == '.')
			tiles.push_back(moveBy(number_anchor, 14, 0, 8, 8));
		else
			tiles.push_back(moveBy(hud_letter, 0, -5, 8, 8));	// empty space
	}

	return new TiledSprite(_spriteSheets["hud"], tiles, size);
}