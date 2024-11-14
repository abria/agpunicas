// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditableObject.h"
#include "Scene.h"
#include "graphicsUtils.h"
#include "sdlUtils.h"
#include "Game.h"
#include "Window.h"
#include "Sprite.h"

using namespace agp;

EditableObject::EditableObject(Scene* scene, const RectF& rect, int category)
	: RenderableObject(scene, rect, nullptr, 1)
{
	_category = category;

	updateColor();
}

EditableObject::EditableObject(Scene* scene, const nlohmann::json& j)
	: RenderableObject(scene, RectF(), nullptr, 1)
{
	_category = j["category"];
	_rect.pos.x = j["rect"]["x"];
	_rect.pos.y = j["rect"]["y"];
	_rect.size.x = j["rect"]["width"];
	_rect.size.y = j["rect"]["height"];
	_rect.yUp = j["rect"]["yUp"];

	updateColor();
}

void EditableObject::updateColor()
{
	Color col = distinctColor(_category);
	setColor(col.adjustAlpha(128));
	setBorderColor(col);
}

void EditableObject::setCategory(int newCategory)
{ 
	_category = newCategory; 
	updateColor();
}


nlohmann::ordered_json EditableObject::toJson()
{
	nlohmann::ordered_json j;
	j["category"] = _category;
	j["rect"]["x"] = _rect.pos.x;
	j["rect"]["y"] = _rect.pos.y;
	j["rect"]["width"] = _rect.size.x;
	j["rect"]["height"] = _rect.size.y;
	j["rect"]["yUp"] = _rect.yUp;

	return j;
}