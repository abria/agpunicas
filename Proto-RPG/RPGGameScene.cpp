// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "RPGGameScene.h"
#include "View.h"
#include "Link.h"
#include "Menu.h"
#include "Game.h"
#include "timeUtils.h"
#include "HUD.h"
#include "RPGGame.h"
#include "HUD.h"
#include "DialogBox.h"

using namespace agp;

RPGGameScene::RPGGameScene(const RectF& rect, const Point& pixelUnitSize, float dt)
	: GameScene(rect, pixelUnitSize, dt)
{
	// SNES aspect ratio
	_view->setRect(RectF(0, 0, 16, 14));
}

void RPGGameScene::updateControls(float timeToSimulate)
{
	if (_cameraManual)
		return;

	const Uint8* keyboard = SDL_GetKeyboardState(0);
	Direction xDir = Direction::NONE;
	Direction yDir = Direction::NONE;
	if (keyboard[SDL_SCANCODE_RIGHT] && !keyboard[SDL_SCANCODE_LEFT])
		xDir = Direction::RIGHT;
	if (keyboard[SDL_SCANCODE_LEFT] && !keyboard[SDL_SCANCODE_RIGHT])
		xDir = Direction::LEFT;
	if (keyboard[SDL_SCANCODE_UP] && !keyboard[SDL_SCANCODE_DOWN])
		yDir = Direction::UP;
	if (keyboard[SDL_SCANCODE_DOWN] && !keyboard[SDL_SCANCODE_UP])
		yDir = Direction::DOWN;
	dynamic_cast<Link*>(_player)->move(xDir, yDir);
}

void RPGGameScene::event(SDL_Event& evt)
{
	GameScene::event(evt);

	Link* link = dynamic_cast<Link*>(_player);

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_RETURN)
		HUD::instance()->inventory(true);
	else if	(evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		Game::instance()->pushScene(Menu::pauseMenu());
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_A && !evt.key.repeat)
		link->attack();
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_T && !evt.key.repeat)
		Game::instance()->pushScene(new DialogBox(
			"The wizard, Agahnim, has done... something to the other missing girls! "
		    "Now only I remain...\n"
		    "Agahnim has seized control of the castle and is now trying to open "
		    "the seven wise men's seal.\n"
		    "I am in the dungeon of the castle.\n"
		    "Would you help me?"));
}