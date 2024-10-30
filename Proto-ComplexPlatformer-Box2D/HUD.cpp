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
	: UIScene(RectF(0, 0, 36, 20), { 32,32 })
{
	setBackgroundColor(Color(0, 0, 0, 0));

	_score = 0;
	_time = 400;
	_fps = 0;

	new RenderableObject(this, RectF(1.5, 0.5, 2.5, 0.5), SpriteFactory::instance()->getText("Score", {0.5f, 0.5f}));
	new RenderableObject(this, RectF(5.5, 0.5, 2, 0.5), SpriteFactory::instance()->getText("FPS", { 0.5f, 0.5f }));
	new RenderableObject(this, RectF(8, 0.5, 2, 0.5), SpriteFactory::instance()->getText("TIME", { 0.5f, 0.5f }));

	_scoreObj = new RenderableObject(this, RectF(1.5, 1, 3, 0.5), SpriteFactory::instance()->getText(std::to_string(_score), { 0.5f, 0.5f }, 6, '0'));
	_fpsObj = new RenderableObject(this, RectF(5.5, 1, 2, 0.5), SpriteFactory::instance()->getText(std::to_string(_fps), { 0.5f, 0.5f }, 2, '0'));
	_timeObj = new RenderableObject(this, RectF(8.5, 1, 1.5, 0.5), SpriteFactory::instance()->getText(std::to_string(int(round(_time))), { 0.5f, 0.5f }, 3, '0'));

	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, 0, 36, 20));
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