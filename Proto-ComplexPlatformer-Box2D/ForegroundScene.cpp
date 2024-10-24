// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ForegroundScene.h"
#include "RenderableObject.h"
#include "Sprite.h"
#include "View.h"
#include "Game.h"

using namespace agp;

ForegroundScene::ForegroundScene(GameScene* gameScene, Sprite* bgSprite, const Vec2Df& parallaxVel, bool seamless)
	: Scene(gameScene->rect())
{
	_gameScene = gameScene;
	_parallaxVel = parallaxVel;
	_seamless = seamless;
	_view = new View(this, gameScene->view()->rect());
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());

	// foreground is repeated as needed if seamless is active
	RectF foreground_rect = view()->rect();
	if(seamless)
	{
		foreground_rect = gameScene->rect() * _parallaxVel;
		foreground_rect.adjust(0, 0, view()->rect().size.x, 0);
	}
	_foreground = new RenderableObject(this, foreground_rect, bgSprite);
}

// implements parallax logic
void ForegroundScene::update(float timeToSimulate)
{
	Scene::update(timeToSimulate);

	if (!_active)
		return;

	// foreground animation
	_foreground->update(timeToSimulate);

	// parallax
	if (_parallaxVel != Vec2Df{ 0,0 })
		_view->setPos(_parallaxVel * _gameScene->view()->rect().pos);
}