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
#include "shaderUtils.h"
#include "CPUShaderWindow.h"
#include "StaticObject.h"

using namespace agp;

RPGGameScene::RPGGameScene(const RectF& rect, const Point& pixelUnitSize, float dt)
	: GameScene(rect, pixelUnitSize, dt)
{
	_transitionEnter = false;
	_transitionExit = false;
	_transitionCounter = 0;
	
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
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.scancode == SDL_SCANCODE_I && !evt.key.repeat)
		link->interact();
	else if (evt.key.keysym.scancode == SDL_SCANCODE_L && !evt.key.repeat)
	{
		if (evt.type == SDL_KEYDOWN)
		{
			PointF center = view()->mapFromScene(player()->rect().center());
			dynamic_cast<CPUShaderWindow*>(Game::instance()->window())->setShader(
				[center](Uint32* pixels, int width, int height, int pitch)
				{
					lightShader(pixels, width, height, pitch, center.x, center.y);
				});
		}
		else if (evt.type == SDL_KEYUP)
			dynamic_cast<CPUShaderWindow*>(Game::instance()->window())->setShader(nullptr);
	}
}

void RPGGameScene::displayGameSceneOnly(bool on) 
{ 
	GameScene::displayGameSceneOnly(on);

	HUD::instance()->setVisible(!on);
}

void RPGGameScene::setTransitionEnter(bool active) 
{ 
	if (_transitionEnter && !active)
	{
		_transitionCounter = 0;
		dynamic_cast<CPUShaderWindow*>(Game::instance()->window())->setShader(nullptr);
	}
	_transitionEnter = active; 
}

void RPGGameScene::setTransitionExit(bool active) 
{ 
	if (_transitionExit && !active)
	{
		_transitionCounter = 0;
		dynamic_cast<CPUShaderWindow*>(Game::instance()->window())->setShader(nullptr);
	}
	_transitionExit = active; 
}

void RPGGameScene::update(float timeToSimulate)
{
	GameScene::update(timeToSimulate);

	if (_transitionEnter || _transitionExit)
	{
		_transitionCounter += timeToSimulate;
		float progress = (std::min)(_transitionCounter, 1.0f);
		float radius = _transitionEnter ? 1 - progress : progress;
		PointF center = view()->mapFromScene(player()->rect().center());

		dynamic_cast<CPUShaderWindow*>(Game::instance()->window())->setShader(
			[center, radius](Uint32* pixels, int width, int height, int pitch)
			{
				circleMaskShader(pixels, width, height, pitch, center.x, center.y, radius);
			});
	}		
}

bool RPGGameScene::isEmpty(const RectF& rect)
{
	for (auto& layer : _sortedObjects)
		for (auto& obj : layer.second)
			if (dynamic_cast<StaticObject*>(obj) && obj->intersectsRect(rect))
				return false;

	return true;
}