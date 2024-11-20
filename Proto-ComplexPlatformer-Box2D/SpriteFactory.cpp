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
	_spriteSheets["hud"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/hud.png", { 147, 187, 236 });
	_spriteSheets["bg_sky"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/bg_sky.png");
	_spriteSheets["bg_houses"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/bg_house.png");
	_spriteSheets["bg_grass"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/bg_grass.png");
	_spriteSheets["fg_fog"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/fg_fog.png");
	_spriteSheets["fg_ground"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/level_0.png");
	_spriteSheets["gear"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/gear.png");
	_spriteSheets["box"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/box.png");
	_spriteSheets["rain"] = loadTextureSequence(renderer, std::string(SDL_GetBasePath()) + "sprites/rain", _autoTiles["rain"], Point(), Point(), "*.png", { 0,0,0 });
	_spriteSheets["player_idle"] = loadTextureSequence(renderer, std::string(SDL_GetBasePath()) + "sprites/PlayerIdle", _autoTiles["player_idle"], { 4 * 32, 3 * 32 }, { -4 * 32, -3 * 32 });
	_spriteSheets["player_walk"] = loadTextureSequence(renderer, std::string(SDL_GetBasePath()) + "sprites/PlayerWalk", _autoTiles["player_walk"], { 4 * 32, 3 * 32 }, { -4 * 32, -3 * 32 });
	_spriteSheets["player_jump"] = loadTextureSequence(renderer, std::string(SDL_GetBasePath()) + "sprites/PlayerJump", _autoTiles["player_jump"], { 4 * 32, 3 * 32 }, { -4 * 32, -3 * 32 });
	_spriteSheets["player_dash"] = loadTextureSequence(renderer, std::string(SDL_GetBasePath()) + "sprites/PlayerDash/DashEffect", _autoTiles["player_dash"], { 4 * 32, 3 * 32 }, { -4 * 32, -3 * 32 });
	_spriteSheets["slime"] = loadTextureSequence(renderer, std::string(SDL_GetBasePath()) + "sprites/Slime", _autoTiles["slime"]);
	_spriteSheets["fire"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/fire.png");
}

// anchors
static RectI hud_letter(519, 254, 8, 8);
static RectI hud_number(519, 263, 8, 8);
static RectI hud_letter_disabled(519, 366, 8, 8);
static RectI hud_number_disabled(519, 375, 8, 8);
static RectI hud_coin(519, 289, 8, 8);

Sprite* SpriteFactory::get(const std::string& id)
{
	std::vector< RectI> rects;

	// static sprites
	if (id == "bg_sky")
		return new Sprite(_spriteSheets["bg_sky"]);
	else if (id == "bg_houses")
		return new FilledSprite(_spriteSheets["bg_houses"]);
	else if (id == "bg_grass")
		return new FilledSprite(_spriteSheets["bg_grass"]);
	else if (id == "fg_fog")
		return new FilledSprite(_spriteSheets["fg_fog"], RectI(0, 0, 1024, 416));
	else if (id == "fg_ground")
		return new Sprite(_spriteSheets["fg_ground"]);
	else if (id == "gear")
		return new Sprite(_spriteSheets["gear"]);
	else if (id == "box")
		return new Sprite(_spriteSheets["box"]);

	// animated sprites
	else if (id == "player_idle")
		return new AnimatedSprite(_spriteSheets[id], _autoTiles[id], 10);
	else if (id == "player_walk")
		return new AnimatedSprite(_spriteSheets[id], _autoTiles[id], 10);
	else if (id == "player_jump")
		return new AnimatedSprite(_spriteSheets[id], _autoTiles[id], 10);
	else if (id == "player_dash")
		return new AnimatedSprite(_spriteSheets[id], _autoTiles[id], 10);
	else if (id == "slime")
		return new AnimatedSprite(_spriteSheets[id], _autoTiles[id], 20);
	else if (id == "rain")
		return new AnimatedSprite(_spriteSheets[id], _autoTiles[id], 30);
	else if (id == "fire")
	{
		RectI baseRect(0, 0, 512, 512);
		rects.push_back(baseRect);
		rects.push_back(moveBy(baseRect, 1, 0, 512, 512, 0, 0));
		rects.push_back(moveBy(baseRect, 2, 0, 512, 512, 0, 0));
		rects.push_back(moveBy(baseRect, 3, 0, 512, 512, 0, 0));
		rects.push_back(moveBy(baseRect, 4, 0, 512, 512, 0, 0));
		rects.push_back(moveBy(baseRect, 5, 0, 512, 512, 0, 0));
		for (auto& r : rects)
			r.scaleOnCenter(0.5f);
		return new AnimatedSprite(_spriteSheets[id], rects, 24);
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