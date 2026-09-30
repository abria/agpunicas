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
#include "Window.h"
#include "AnimatedSprite.h"
#include "TiledSprite.h"
#include "FilledSprite.h"
#include "Game.h"
#include <iostream>

using namespace agp;

SpriteFactory::SpriteFactory()
{
SDL_Renderer* renderer = Game::instance()->window()->renderer();

	_spriteSheets["mario"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/mario.png", _autoTiles["mario"], { 41, 88, 124 }, { 68, 145, 190 });
	_spriteSheets["titlescreen"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/titlescreen.png", {224, 163, 216});
	_spriteSheets["font"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/font.png", { 94, 219, 146 });
	_spriteSheets["hud"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/hud.png");
	_spriteSheets["stages"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/stages.png", { 224, 163, 216 });
	_spriteSheets["items"] = loadTextureAutoDetect(renderer, std::string(SDL_GetBasePath()) + "sprites/items.png", _autoTiles["items"], { 41, 88, 124 }, { 68, 145, 190 });
	_spriteSheets["enemies"] = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/enemies.png", { 68, 145, 190 });
}

Sprite* SpriteFactory::loadLevelBackground(const std::string& fileName)
{
	SDL_Renderer* renderer = Game::instance()->window()->renderer();
	SDL_Texture* levelTexture = loadTexture(renderer, std::string(SDL_GetBasePath()) + "sprites/levels/" + fileName);
	
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
	if (id == "level_1_1")
		return loadLevelBackground("1-1.png");
	std::vector< RectI> rects;

	// screens
	if (id == "welcome")
		return new Sprite(_spriteSheets["titlescreen"], RectI(783, 244, 16 * 16, 16 * 14));
	else if (id == "flashing_3")
	{
		RectI welcome3(115, 574, 32, 48);
		rects.push_back(welcome3);
		rects.push_back(welcome3 + Point(1 * 260, 0));
		rects.push_back(welcome3 + Point(2 * 260, 0));
		rects.push_back(welcome3 + Point(3 * 260, 0));
		return new AnimatedSprite(_spriteSheets["titlescreen"], rects, 10, {0, 1, 0, 2, 3, 2, 0});
	}

	// points
	else if (id.rfind("points_", 0) == 0 && id.size() == 8)
	{
		int point_idx = id[7] - '0';
		if (point_idx < 4)
			return new Sprite(_spriteSheets["font"], moveBy(RectI(60, 89, 12, 8), point_idx, 0, 12, 8, 3));
		else if (point_idx < 9)
			return new Sprite(_spriteSheets["font"], moveBy(RectI(119, 89, 16, 8), point_idx-4, 0, 16, 8, 3));
		else
			return nullptr;
	}

	// HUD
	else if (id == "hud")
		return new Sprite(_spriteSheets["hud"], RectI(4, 81, 16 * 16, 16 * 3));
	else if (id == "hud_arrow")
		return new Sprite(_spriteSheets["hud"], RectI(625, 25, 8, 8));
	else if (id == "hud_P")
		return new Sprite(_spriteSheets["hud"], RectI(653, 25, 16, 8));

	// Mario
	else if (id == "mario_stand")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][0]);
	else if (id == "mario_jump")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][2]);
	else if (id == "mario_jumpP")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][5]);
	else if (id == "mario_fall")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][2]);
	else if (id == "mario_skid")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][6]);
	else if (id == "mario_die")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][0][17]);
	else if (id == "mario_walk")
		return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin(), _autoTiles["mario"][0].begin() + 2 }, 10);
	else if (id == "mario_run")
		return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin(), _autoTiles["mario"][0].begin() + 2 }, 30);
	else if (id == "mario_pspeed")
		return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][0].begin() + 3, _autoTiles["mario"][0].begin() + 5 }, 30);
	else if (id == "mario_small2big")
	{
		rects.push_back(_autoTiles["mario"][3][8]);
		rects.push_back(_autoTiles["mario"][0][0]);
		rects.push_back(_autoTiles["mario"][2][0]);
		return new AnimatedSprite(_spriteSheets["mario"], rects, 10, { 0, 1, 0, 1, 0, 1, 0, 2, 0, 2, 0, 2 }, 1);
	}
	else if (id == "supermario_big2small")
	{
		rects.push_back(_autoTiles["mario"][3][8]);
		rects.push_back(_autoTiles["mario"][0][0]);
		rects.push_back(_autoTiles["mario"][2][0]);
		return new AnimatedSprite(_spriteSheets["mario"], rects, 10, { 2, 0, 2, 0, 2, 0, 1, 0, 1, 0, 1, 0, 1 }, 1);
	}

	// Super Mario
	else if (id == "supermario_stand")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][0]);
	else if (id == "supermario_jump")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][4]);
	else if (id == "supermario_jumpP")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][8]);
	else if (id == "supermario_fall")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][2]);
	else if (id == "supermario_crouch")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][3]);
	else if (id == "supermario_skid")
		return new Sprite(_spriteSheets["mario"], _autoTiles["mario"][2][9]);
	else if (id == "supermario_walk")
		return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][2].begin(), _autoTiles["mario"][2].begin() + 3 }, 10, { 0, 1, 2, 1 });
	else if (id == "supermario_run")
		return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][2].begin(), _autoTiles["mario"][2].begin() + 3 }, 30, { 0, 1, 2, 1 });
	else if (id == "supermario_pspeed")
		return new AnimatedSprite(_spriteSheets["mario"], { _autoTiles["mario"][2].begin() + 5, _autoTiles["mario"][2].begin() + 8 }, 30, { 0, 1, 2, 1 });

	// enemies
	else if (id == "goomba_walk")
	{
		RectI anchor(1, 1, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, 1, 0, 16, 16, 1, 1));
		return new AnimatedSprite(_spriteSheets["enemies"], rects, 8);
	}
	else if (id == "goomba_stomped")
		return new Sprite(_spriteSheets["enemies"], RectI(35, 1, 16, 16));

	// level objects
	else if (id == "block_question")
	{
		RectI anchor(104, 235, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, 1, 0, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, -1, 1, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 0, 1, 16, 16, 2, 2));
		return new AnimatedSprite(_spriteSheets["stages"], rects, 8);
	}
	else if (id == "block_empty")
		return new Sprite(_spriteSheets["stages"], RectI(122, 199, 16, 16));
	else if (id == "brick")
	{
		RectI anchor(122, 253, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, -2, 1, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, -1, 1, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 0, 1, 16, 16, 2, 2));
		return new AnimatedSprite(_spriteSheets["stages"], rects, 8);
	}
	else if (id == "brick_debris")
		return new AnimatedSprite(_spriteSheets["items"], { _autoTiles["items"][4].begin() + 4, _autoTiles["items"][4].begin() + 6 }, 10);
	else if (id == "coin")
	{
		RectI anchor(86, 217, 16, 16);
		rects.push_back(anchor);
		rects.push_back(moveBy(anchor, 1, 0, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 2, 0, 16, 16, 2, 2));
		rects.push_back(moveBy(anchor, 0, 1, 16, 16, 2, 2));
		return new AnimatedSprite(_spriteSheets["stages"], rects, 8);
	}
	else if (id == "wood")
		return new Sprite(_spriteSheets["stages"], RectI(104, 199, 16, 16));
	else if (id == "spawnable_coin")
		return new AnimatedSprite(_spriteSheets["items"], { _autoTiles["items"][0].begin() + 19, _autoTiles["items"][0].begin() + 22 }, 16, { 0, 1, 2, 1 });
	else if (id == "mushroom_red")
		return new Sprite(_spriteSheets["items"], _autoTiles["items"][0][4]);
	else if (id == "leaf")
		return new Sprite(_spriteSheets["items"], _autoTiles["items"][0][5]);
	else if (id == "mushroom_green")
		return new Sprite(_spriteSheets["items"], _autoTiles["items"][0][7]);
	else
	{
		std::cerr << "Cannot find sprite \"" << id << "\"\n";
		return nullptr;
	}
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