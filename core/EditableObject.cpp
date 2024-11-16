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
#include "TextSprite.h"

using namespace agp;

EditableObject::EditableObject(Scene* scene, const RectF& rect, const std::string& name, int category, std::vector<std::string>& categories)
	: RenderableObject(scene, rect, nullptr, 1), _categories(categories)
{
	_category = category;
	_name = name;
	_selected = false;

	init();
}

EditableObject::EditableObject(Scene* scene, const nlohmann::json& j, std::vector<std::string>& categories)
	: RenderableObject(scene, RectF(), nullptr, 1), _categories(categories)
{
	_category = j["category"];
	_name = j["name"];
	_rect.pos.x = j["rect"]["x"];
	_rect.pos.y = j["rect"]["y"];
	_rect.size.x = j["rect"]["width"];
	_rect.size.y = j["rect"]["height"];
	_rect.yUp = j["rect"]["yUp"];
	_selected = false;

	init();
}

void EditableObject::init()
{
	Color col = distinctColor(_category);
	setColor(col.adjustAlpha(160));
	setBorderColor(col);

	_renderedText = new RenderableObject(
		_scene, 
		_rect,
		new TextSprite(_name, "font.ttf", col.brighter(), {0.1f, 0.1f},
			Direction::UP, TextSprite::VAlign::CENTER, TextSprite::HAlign::CENTER), 2);

	_renderedCategory = new RenderableObject(
		_scene,
		_rect,
		new TextSprite(_categories[_category], "font.ttf", col.brighter(), {_rect.size.x / 4, 0},
			Direction::UP, TextSprite::VAlign::TOP, TextSprite::HAlign::CENTER, TextSprite::Style::ITALIC), 2);
}

EditableObject::~EditableObject()
{
	if (_renderedText)
		_renderedText->kill();
	if (_renderedCategory)
		_renderedCategory->kill();
}

void EditableObject::setCategory(int newCategory)
{ 
	_category = newCategory; 

	Color col = distinctColor(_category);
	setColor(col.adjustAlpha(160));
	setBorderColor(col);

	dynamic_cast<TextSprite*>(_renderedCategory->sprite())->setText(_categories[_category]);
}

void EditableObject::setName(const std::string& name)
{ 
	_name = name;
	dynamic_cast<TextSprite*>(_renderedText->sprite())->setText(_name);
}

nlohmann::ordered_json EditableObject::toJson()
{
	nlohmann::ordered_json j;
	j["category"] = _category;
	j["name"] = _name;
	j["rect"]["x"] = _rect.pos.x;
	j["rect"]["y"] = _rect.pos.y;
	j["rect"]["width"] = _rect.size.x;
	j["rect"]["height"] = _rect.size.y;
	j["rect"]["yUp"] = _rect.yUp;

	return j;
}

void EditableObject::setFocused(bool on)
{
	if (_selected)
		return;

	if (on)
		_color.a = 200;
	else
		_color.a = 160;

	setBorderThickness(on ? 5.0f : 0);
}

void EditableObject::setSelected(bool on)
{
	if (on)
		_color.a = 100;
	else
		_color.a = 160;

	setBorderThickness(on ? 5.0f : 0);

	_selected = on;
}

void EditableObject::setVisible(bool visible)
{
	RenderableObject::setVisible(visible);
	_renderedText->setVisible(visible);
	_renderedCategory->setVisible(visible);
}

void EditableObject::setPos(const PointF& newPos)
{
	RenderableObject::setPos(newPos);
	_renderedText->setPos(newPos);
	_renderedCategory->setPos(newPos);
}

void EditableObject::setSize(const PointF& newSize)
{ 
	RenderableObject::setSize(newSize);
	_renderedText->setRect(_rect);
	_renderedCategory->setRect(_rect);
	dynamic_cast<TextSprite*>(_renderedCategory->sprite())->setMargin({ _rect.size.x / 4, 0 });
}