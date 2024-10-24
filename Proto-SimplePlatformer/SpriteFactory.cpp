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
	_spriteSheets["mario"] = loadTexture(renderer, "sprites/mario.png", { 147, 187, 236 });
	_spriteSheets["enemies"] = loadTexture(renderer, "sprites/enemies.png", { 147, 187, 236 });
	_spriteSheets["hud"] = loadTexture(renderer, "sprites/hud.png", { 147, 187, 236 });
	_spriteSheets["tiles"] = loadTexture(renderer, "sprites/stage_tiles.png", { 147, 187, 236 });
}

// anchors
static RectI mario_small(1, 9, 16, 16);
static RectI hud_letter(519, 254, 8, 8);
static RectI hud_number(519, 263, 8, 8);
static RectI hud_letter_disabled(519, 366, 8, 8);
static RectI hud_number_disabled(519, 375, 8, 8);
static RectI hud_coin(519, 289, 8, 8);
static RectI flower(1, 12, 16, 16);
static RectI goomba(1, 28, 16, 16);
static RectI koopa(52, 12, 16, 32);

Sprite* SpriteFactory::get(const std::string& id)
{
	std::vector< RectI> rects;

	// single-frame sprites
	if (id == "terrain")
		return new FilledSprite(_spriteSheets["tiles"], moveBy(flower, 0, 7));
	else if (id == "brick")
		return new Sprite(_spriteSheets["tiles"], moveBy(flower, 1, 5));
	else if (id == "wall")
		return new Sprite(_spriteSheets["tiles"], moveBy(flower, 2, 5));
	else if (id == "block")
		return new Sprite(_spriteSheets["tiles"], moveBy(flower, 5, 3));
	else if (id == "welcome")
		return new Sprite(_spriteSheets["hud"], RectI(1, 2 + 16 * 2, 16 * 16, 13 * 16));
	else if (id == "gameover")
		return new Sprite(_spriteSheets["hud"], RectI(260, 253, 16 * 16, 15 * 16));
	else if (id == "mario_stand")
		return new Sprite(_spriteSheets["mario"], mario_small);
	else if (id == "mario_jump")
		return new Sprite(_spriteSheets["mario"], moveBy(mario_small, 6, 0));
	else if (id == "mario_skid")
		return new Sprite(_spriteSheets["mario"], moveBy(mario_small, 5, 0));
	else if (id == "mario_die")
		return new Sprite(_spriteSheets["mario"], moveBy(mario_small, 1, 0));
	else if (id == "hammer")
		return new Sprite(_spriteSheets["enemies"], moveBy(goomba, 25, 0));
	else if (id == "hammer_brother_jump")
		return new Sprite(_spriteSheets["enemies"], moveBy(koopa, 19, 0));
	else if (id == "platform")
		return new FilledSprite(_spriteSheets["tiles"], RectI(120, 148, 8, 8), {0.5f, 0.5f});

	// animated sprites
	else if (id == "mario_walk")
	{
		rects.push_back(moveBy(mario_small, 2, 0));
		rects.push_back(moveBy(mario_small, 3, 0));
		rects.push_back(moveBy(mario_small, 4, 0));
		return new AnimatedSprite(_spriteSheets["mario"], rects, 10);
	}
	else if (id == "mario_run")
	{
		rects.push_back(moveBy(mario_small, 2, 0));
		rects.push_back(moveBy(mario_small, 3, 0));
		rects.push_back(moveBy(mario_small, 4, 0));
		return new AnimatedSprite(_spriteSheets["mario"], rects, 20);
	}
	else if (id == "box")
	{
		rects.push_back(moveBy(flower, 0, 3));
		rects.push_back(moveBy(flower, 1, 3));
		rects.push_back(moveBy(flower, 2, 3));
		rects.push_back(moveBy(flower, 1, 3));
		rects.push_back(moveBy(flower, 0, 3));
		return new AnimatedSprite(_spriteSheets["tiles"], rects, 5);
	}
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
	else if (id == "hammer_brother_walk")
	{
		rects.push_back(moveBy(koopa, 18, 0));
		rects.push_back(moveBy(koopa, 19, 0));
		return new AnimatedSprite(_spriteSheets["enemies"], rects, 8);
	}
	else if (id == "hammer_brother_throw")
	{
		rects.push_back(moveBy(koopa, 20, 0));
		rects.push_back(moveBy(koopa, 21, 0));
		return new AnimatedSprite(_spriteSheets["enemies"], rects, 8);
	}

	// tiled sprites
	if (id == "pipe-small")
	{
		rects.push_back(moveBy(flower, 6, 2));
		rects.push_back(moveBy(flower, 7, 2));
		rects.push_back(moveBy(flower, 6, 3));
		rects.push_back(moveBy(flower, 7, 3));
		return new TiledSprite(_spriteSheets["tiles"], rects);
	}
	else if (id == "pipe-med")
	{
		rects.push_back(moveBy(flower, 6, 2));
		rects.push_back(moveBy(flower, 7, 2));
		rects.push_back(moveBy(flower, 6, 3));
		rects.push_back(moveBy(flower, 7, 3));
		rects.push_back(moveBy(flower, 6, 3));
		rects.push_back(moveBy(flower, 7, 3));
		return new TiledSprite(_spriteSheets["tiles"], rects);
	}
	else if (id == "pipe-big")
	{
		rects.push_back(moveBy(flower, 6, 2));
		rects.push_back(moveBy(flower, 7, 2));
		rects.push_back(moveBy(flower, 6, 3));
		rects.push_back(moveBy(flower, 7, 3));
		rects.push_back(moveBy(flower, 6, 3));
		rects.push_back(moveBy(flower, 7, 3));
		rects.push_back(moveBy(flower, 6, 3));
		rects.push_back(moveBy(flower, 7, 3));
		return new TiledSprite(_spriteSheets["tiles"], rects);
	}

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