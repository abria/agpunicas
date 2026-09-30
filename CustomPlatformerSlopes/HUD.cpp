// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "HUD.h"
#include "SpriteFactory.h"
#include "View.h"
#include "Game.h"
#include "Audio.h"

using namespace agp;

HUD::HUD()
	: UIScene(RectF(0, 0, 16, 15), { 16, 16 })
{
	setBackgroundColor(Color(0, 0, 0, 0));

	_score = 0;
	_coins = 0;
	_world = 1;
	_time = 300;
	_fps = 0;
	_lives = 3;
	_running = false;
	_runningTime = 0;
	_notRunningTime = 1000;
	_pMeter = 0;
	_comboCount = -1;
	_comboTimer = 0.0f;

	new RenderableObject(this, RectF(0, 12, 16, 3), SpriteFactory::instance()->get("hud"), 0);
	new RenderableObject(this, RectF(12, 14.4f, 1.5f, 0.5f), SpriteFactory::instance()->getText("FPS", {0.5f, 0.5f}), 1);
	_fpsObj = new RenderableObject(this, RectF(14, 14.45f, 1.5f, 0.5f), SpriteFactory::instance()->getText(std::to_string(_fps), { 0.5f, 0.5f }, 3, ' '), 1);
	_scoreObj = new RenderableObject(this, RectF(4, 13, 3.5f, 0.5f), SpriteFactory::instance()->getNumber(_score, { 0.5f, 0.5f }, 7), 1);
	_coinsObj = new RenderableObject(this, RectF(9, 12.5f, 1, 0.5f), SpriteFactory::instance()->getNumber(_coins, { 0.5f, 0.5f }, 2, ' '), 1);
	_worldObj = new RenderableObject(this, RectF(3, 12.5f, 0.5f, 0.5f), SpriteFactory::instance()->getNumber(_world, { 0.5f, 0.5f }), 1);
	_livesObj = new RenderableObject(this, RectF(2.5f, 13, 1.0f, 0.5f), SpriteFactory::instance()->getNumber(_lives, { 0.5f, 0.5f }, 2, ' '), 2);
	_timeObj = new RenderableObject(this, RectF(8.5f, 13, 1.5f, 0.5f), SpriteFactory::instance()->getNumber(int(round(_time)), { 0.5f, 0.5f }, 3), 1);

	for (int i = 0; i < 7; i++)
	{
		_pMeterObjects[i] = new RenderableObject(this, RectF(4 + 0.5f * i, 12.5f, i == 6 ? 1 : 0.5f, 0.5f), SpriteFactory::instance()->get(i == 6 ? "hud_P" : "hud_arrow"), 1);
		_pMeterObjects[i]->setVisible(false);
	}

	// setup view (specific for SMB3)
	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, 0, 16, 15));
}

// extends update logic (+time management)
void HUD::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	if (!_active)
		return;

	// update time remaining
	int timePrev = int(round(_time));
	_time -= timeToSimulate;
	int timeCurr = int(round(_time));
	if(timePrev != timeCurr)
		_timeObj->setSprite(SpriteFactory::instance()->getNumber(timeCurr, { 0.5f, 0.5f }, 3));

	// update p-meter
	if (_running) 
	{
		_runningTime += timeToSimulate;
		_notRunningTime = 0.0f;

		while (_runningTime >= 0.2f) 
		{
			_runningTime -= 0.2f;
			_pMeter = std::min(_pMeter + 1, 7);
		}
	}
	else 
	{
		_notRunningTime += timeToSimulate;
		_runningTime = 0.0f;

		while (_notRunningTime >= 0.4f) 
		{
			_notRunningTime -= 0.4f;
			_pMeter = std::max(_pMeter - 1, 0);
		}
	}
	for (int i = 0; i < 6; i++)
		_pMeterObjects[i]->setVisible(i < _pMeter);
	_pMeterObjects[6]->setVisible(_pMeter == 7);
	_pMeterObjects[6]->setFlashingFrequency(_pMeter == 7 ? 1 : 0);

	// update combo
	_comboTimer = std::max(0.0f, _comboTimer - timeToSimulate);
	if (!_comboTimer)
		_comboCount = -1;

	setFPS(Game::instance()->currentFPS());
}

void HUD::setRunning(bool on)
{
	_running = on;
}

void HUD::setFPS(int fps) 
{ 
	if (fps != _fps)
	{
		_fps = fps;
		_fpsObj->setSprite(SpriteFactory::instance()->getText(std::to_string(_fps), { 0.5f, 0.5f }, 3, ' '));
	}
}

void HUD::addCoin()
{
	_coins += 1;
	addScore(50);

	Audio::instance()->playSound("Coin", 0, true);

	if (_coins == 100)
	{
		addLife();
		_coins = 0;
	}

	_coinsObj->setSprite(SpriteFactory::instance()->getNumber(_coins, { 0.5f, 0.5f }, 2, ' '));
}

void HUD::addLife()
{
	_lives = std::min(99, _lives + 1);

	Audio::instance()->playSound("Life");

	_livesObj->setSprite(SpriteFactory::instance()->getNumber(_lives, { 0.5f, 0.5f }, 2, ' '));
}

void HUD::addScore(int score)
{
	_score += score;

	_scoreObj->setSprite(SpriteFactory::instance()->getNumber(_score, { 0.5f, 0.5f }, 7));
}

int HUD::addKill()
{
	_comboTimer = COMBO_WINDOW;

	_comboCount = std::min(_comboCount + 1, 8);

    switch (_comboCount)
    {
        case 0:  addScore(100); break;
        case 1:  addScore(200); break;
        case 2:  addScore(400); break;
        case 3:  addScore(800); break;
        case 4:  addScore(1000); break;
        case 5:  addScore(2000); break;
        case 6:  addScore(4000); break;
		case 7:  addScore(8000); break;
        case 8:  addLife(); break;
    }

	return _comboCount;
}