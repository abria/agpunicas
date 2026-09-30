// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "PlatformerGameScene.h"
#include "View.h"
#include "Mario.h"
#include "Menu.h"
#include "Game.h"
#include "timeUtils.h"
#include "HUD.h"
#include "PlatformerGame.h"

using namespace agp;

PlatformerGameScene::PlatformerGameScene(const RectF& rect, const Point& pixelUnitSize, float dt)
	: GameScene(rect, pixelUnitSize, dt)
{
	// NES aspect ratio (16 x 15)
	_view->setRect(RectF(0, -10, 16, 15));

	_useQuadtree = false;
	_autoKillWhenOutsideScene = false;
}

void PlatformerGameScene::updateControls(float timeToSimulate)
{
	if (_cameraManual)
		return;

	Mario* mario = dynamic_cast<Mario*>(_player);
	const bool* keyboard = SDL_GetKeyboardState(0);

	if (keyboard[SDL_SCANCODE_RIGHT] && !keyboard[SDL_SCANCODE_LEFT] && !keyboard[SDL_SCANCODE_DOWN])
		mario->move(Direction::RIGHT);
	else if (keyboard[SDL_SCANCODE_LEFT] && !keyboard[SDL_SCANCODE_RIGHT] && !keyboard[SDL_SCANCODE_DOWN])
		mario->move(Direction::LEFT);
	else if (keyboard[SDL_SCANCODE_LEFT] && keyboard[SDL_SCANCODE_RIGHT])
		mario->move(Direction::NONE);
	else
		mario->move(Direction::NONE);

	mario->jump(keyboard[SDL_SCANCODE_SPACE]);
	mario->run(keyboard[SDL_SCANCODE_Z]);
	mario->crouch(keyboard[SDL_SCANCODE_DOWN]);
}

void PlatformerGameScene::updateCamera(float timeToSimulate)
{
	if (_cameraManual)
	{
		GameScene::updateCamera(timeToSimulate);
		return;
	}

	Mario* mario = dynamic_cast<Mario*>(_player);


	_view->setX(std::max(mario->rect().pos.x - 8, _view->rect().pos.x));
	_view->setX(std::min(mario->rect().pos.x - 7, _view->rect().pos.x));

	if (_view->rect().left() < 0)
		_view->setX(0);
	if (_view->rect().right() > _rect.right())
		_view->setX(_rect.right() - _view->rect().size.x);
}

void PlatformerGameScene::event(SDL_Event& evt)
{
	GameScene::event(evt);

	Mario* mario = dynamic_cast<Mario*>(_player);

	if (evt.type == SDL_EVENT_KEY_DOWN && (evt.key.scancode == SDL_SCANCODE_RETURN || evt.key.scancode == SDL_SCANCODE_ESCAPE))
		Game::instance()->pushScene(Menu::pauseMenu());

	else if (evt.type == SDL_EVENT_KEY_DOWN && (evt.key.scancode == SDL_SCANCODE_T))
		mario->powerup();
	else if (evt.type == SDL_EVENT_KEY_DOWN && (evt.key.scancode == SDL_SCANCODE_H))
		mario->powerdown();
}