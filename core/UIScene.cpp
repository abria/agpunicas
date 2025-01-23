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

using namespace agp;

UIScene::UIScene(const RectF& rect, const Point& pixelUnitSize)
	: Scene(rect, pixelUnitSize)
{
	_view = new View(this, _rect);
}

void UIScene::update(float timeToSimulate)
{
	Scene::update(timeToSimulate);

	if (_active)
		for (auto& obj : _objects)
			obj->update(timeToSimulate);
}