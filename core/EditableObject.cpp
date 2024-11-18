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

	_renderedName = new RenderableObject(
		_scene,
		_rect,
		new TextSprite(_name, "font.ttf", col.brighter(), { NAME_MARGIN_X, 0.0f }, { 0, NAME_MAX_HEIGHT },
			TextSprite::VAlign::CENTER, TextSprite::HAlign::CENTER), 2);

	_renderedCategory = new RenderableObject(
		_scene,
		_rect,
		new TextSprite(_categories[_category], "font.ttf", col.brighter(), {0.0f, CATEGORY_MARGIN_Y}, { 0, CATEGORY_MAX_HEIGHT },
			TextSprite::VAlign::TOP, TextSprite::HAlign::CENTER, TextSprite::Style::ITALIC), 2);
}

EditableObject::~EditableObject()
{
	if (_renderedName)
		_renderedName->kill();
	if (_renderedCategory)
		_renderedCategory->kill();
}

void EditableObject::setCategory(int newCategory)
{ 
	_category = newCategory; 

	Color col = distinctColor(_category);
	setColor(col.adjustAlpha(ALPHA_NORMAL));
	setBorderColor(col);

	dynamic_cast<TextSprite*>(_renderedCategory->sprite())->setText(_categories[_category]);
}

void EditableObject::setName(const std::string& name)
{ 
	_name = name;
	dynamic_cast<TextSprite*>(_renderedName->sprite())->setText(_name);
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
		_color.a = ALPHA_FOCUSED;
	else
		_color.a = ALPHA_NORMAL;

	setBorderThickness(on ? SELECTED_BORDER_THICKNESS : 0);
}

void EditableObject::setSelected(bool on)
{
	if (on)
		_color.a = ALPHA_SELECTED;
	else
		_color.a = ALPHA_NORMAL;

	setBorderThickness(on ? SELECTED_BORDER_THICKNESS : 0);

	_selected = on;
}

void EditableObject::setVisible(bool visible)
{
	RenderableObject::setVisible(visible);
	_renderedName->setVisible(visible);
	_renderedCategory->setVisible(visible);
}

void EditableObject::setPos(const PointF& newPos)
{
	RenderableObject::setPos(newPos);
	_renderedName->setPos(newPos);
	_renderedCategory->setPos(newPos);
}

void EditableObject::setSize(const PointF& newSize)
{ 
	if (newSize.x <= 0 || newSize.y <= 0)
		return;

	RenderableObject::setSize(newSize);
	_renderedName->setRect(_rect);
	_renderedCategory->setRect(_rect);
}