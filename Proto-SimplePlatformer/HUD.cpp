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

using namespace agp;

HUD::HUD()
	: UIScene(RectF(0, 0, 16, 15))
{
	setBackgroundColor(Color(0, 0, 0, 0));

	_score = 0;
	_coins = 0;
	_world = 1;
	_level = 1;
	_time = 400;
	_fps = 0;

	new RenderableObject(this, RectF(1.5, 0.5, 2.5, 0.5), SpriteFactory::instance()->getText("MARIO", {0.5f, 0.5f}));
	new RenderableObject(this, RectF(5.5, 0.5, 2, 0.5), SpriteFactory::instance()->getText("F-", { 0.5f, 0.5f }));
	new RenderableObject(this, RectF(9.0, 0.5, 2.5, 0.5), SpriteFactory::instance()->getText("WORLD", { 0.5f, 0.5f }));
	new RenderableObject(this, RectF(12.5, 0.5, 2, 0.5), SpriteFactory::instance()->getText("TIME", { 0.5f, 0.5f }));
	new RenderableObject(this, RectF(6.0, 1, 0.5, 0.5), SpriteFactory::instance()->getText("*", { 0.5f, 0.5f }));
	new RenderableObject(this, RectF(10, 1, 0.5, 0.5), SpriteFactory::instance()->getText("-", { 0.5f, 0.5f }));

	_fpsObj = new RenderableObject(this, RectF(6.5, 0.5, 2, 0.5), SpriteFactory::instance()->getText(std::to_string(_fps), { 0.5f, 0.5f }));
	_scoreObj = new RenderableObject(this, RectF(1.5, 1, 3, 0.5), SpriteFactory::instance()->getText(std::to_string(_score), { 0.5f, 0.5f }, 6, '0'));
	_flashingCoinObj = new RenderableObject(this, RectF(5.5, 1, 0.5, 0.5), SpriteFactory::instance()->get("hud_coin"));
	_coinsObj = new RenderableObject(this, RectF(6.5, 1, 1, 0.5), SpriteFactory::instance()->getText(std::to_string(_coins), { 0.5f, 0.5f }, 2, '0'));
	_worldObj = new RenderableObject(this, RectF(9.5, 1, 0.5, 0.5), SpriteFactory::instance()->getText(std::to_string(_world), { 0.5f, 0.5f }));
	_levelObj = new RenderableObject(this, RectF(10.5, 1, 0.5, 0.5), SpriteFactory::instance()->getText(std::to_string(_level), { 0.5f, 0.5f }));
	_timeObj = new RenderableObject(this, RectF(13, 1, 1.5, 0.5), SpriteFactory::instance()->getText(std::to_string(int(round(_time))), { 0.5f, 0.5f }, 3, '0'));

	// setup view (specific for super mario bros)
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

	int timePrev = int(round(_time));
	_time -= timeToSimulate;
	int timeCurr = int(round(_time));
	if(timePrev != timeCurr)
		_timeObj->setSprite(SpriteFactory::instance()->getText(std::to_string(timeCurr), { 0.5f, 0.5f }, 3, '0'));
}

void HUD::setFPS(int fps) 
{ 
	_fps = fps; 
	_fpsObj->setSprite(SpriteFactory::instance()->getText(std::to_string(_fps), { 0.5f, 0.5f }));
}