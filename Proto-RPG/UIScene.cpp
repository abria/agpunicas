// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScene.h"
#include "Object.h"
#include "View.h"
#include "Game.h"

using namespace agp;

UIScene::UIScene(const RectF& r)
	: Scene(r)
{
	// setup view (specific for super mario bros)
	_view = new View(this, _rect);
	//_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, 0, 16, 15));
}

void UIScene::update(float timeToSimulate)
{
	Scene::update(timeToSimulate);

	if (_active)
		for (auto& layer : _sortedObjects)
			for (auto& obj : layer.second)
				obj->update(timeToSimulate);
}