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
	_rotRect.center = _rect.center();
	_rotRect.size = _rect.size;
	_rotRect.angle = 0;
	_rotRect.yUp = _rect.yUp;
	_resizingEdgeIndex = -1;

	init();
}

EditableObject::EditableObject(Scene* scene, const nlohmann::json& j, std::vector<std::string>& categories)
	: RenderableObject(scene, RectF(), nullptr, 1), _categories(categories)
{
	_category = j["category"];
	_name = j["name"];

	if (j.contains("rect"))
	{
		_rect.pos.x = j["rect"]["x"];
		_rect.pos.y = j["rect"]["y"];
		_rect.size.x = j["rect"]["width"];
		_rect.size.y = j["rect"]["height"];
		_rect.yUp = j["rect"]["yUp"];
		_rotRect.center = _rect.center();
		_rotRect.size = _rect.size;
		_rotRect.angle = 0;
		_rotRect.yUp = _rect.yUp;
	}
	else if (j.contains("rotRect"))
	{
		_rotRect.center.x = j["rotRect"]["cx"];
		_rotRect.center.y = j["rotRect"]["cy"];
		_rotRect.size.x = j["rotRect"]["width"];
		_rotRect.size.y = j["rotRect"]["height"];
		_rotRect.angle = j["rotRect"]["angle"];
		_rotRect.yUp = j["rotRect"]["yUp"];
		_rect = _rotRect.toRect();
	}

	_selected = false;

	init();
}

nlohmann::ordered_json EditableObject::toJson()
{
	nlohmann::ordered_json j;
	j["category"] = _category;
	j["name"] = _name;

	if (_rotRect.angle)
	{
		j["rotRect"]["cx"] = _rotRect.center.x;
		j["rotRect"]["cy"] = _rotRect.center.y;
		j["rotRect"]["width"] = _rotRect.size.x;
		j["rotRect"]["height"] = _rotRect.size.y;
		j["rotRect"]["angle"] = _rotRect.angle;
		j["rotRect"]["yUp"] = _rotRect.yUp;
	}
	else
	{
		j["rect"]["x"] = _rect.pos.x;
		j["rect"]["y"] = _rect.pos.y;
		j["rect"]["width"] = _rect.size.x;
		j["rect"]["height"] = _rect.size.y;
		j["rect"]["yUp"] = _rect.yUp;
	}

	return j;
}

void EditableObject::init()
{
	Color col = distinctColor(_category);
	setColor(col.adjustAlpha(160));
	setBorderColor(col);

	_renderedName = new RenderableObject(
		_scene,
		_rect,
		new TextSprite(_name, std::string(SDL_GetBasePath()) + "fonts/font.ttf", col.brighter(), { NAME_MARGIN_X, 0.0f }, { 0, NAME_MAX_HEIGHT },
			TextSprite::VAlign::CENTER, TextSprite::HAlign::CENTER), 2);
	_renderedName->setAngle(-_rotRect.angle);

	_renderedCategory = new RenderableObject(
		_scene,
		_rect,
		new TextSprite(_categories[_category], std::string(SDL_GetBasePath()) + "fonts/font.ttf", col.brighter(), {0.0f, CATEGORY_MARGIN_Y}, { 0, CATEGORY_MAX_HEIGHT },
			TextSprite::VAlign::TOP, TextSprite::HAlign::CENTER, TextSprite::Style::ITALIC), 2);
	_renderedCategory->setAngle(-_rotRect.angle);
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
	dynamic_cast<TextSprite*>(_renderedCategory->sprite())->setColor(col.brighter());
	dynamic_cast<TextSprite*>(_renderedName->sprite())->setColor(col.brighter());
}

void EditableObject::setName(const std::string& name)
{ 
	_name = name;
	dynamic_cast<TextSprite*>(_renderedName->sprite())->setText(_name);
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

bool EditableObject::contains(const Vec2Df& p)
{
	if (_rotRect.angle == 0)
		return RenderableObject::contains(p);
	else
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		return rotRectRadians.contains(p);
	}
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
	_rotRect.center = _rect.center();
}

void EditableObject::setSize(const PointF& newSize)
{ 
	if (newSize.x <= 0 || newSize.y <= 0)
		return;

	RenderableObject::setSize(newSize);
	_renderedName->setRect(_rect);
	_renderedCategory->setRect(_rect);

	_rotRect.center = _rect.center();
	_rotRect.size = _rect.size;
}

bool EditableObject::resizableAt(const PointF& point)
{
	float minDist = inf<float>();
	if (_rotRect.angle)
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		_resizingEdgeIndex = closestEdgeIndex(rotRectRadians.vertices(), point, minDist);
	}
	else
		_resizingEdgeIndex = closestEdgeIndex(_rect.vertices(), point, minDist);

	return minDist < RESIZING_HOOK_DISTANCE;
}

SDL_SystemCursor EditableObject::resizeCursor()
{
	if (_rotRect.angle)
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		auto vertices = rotRectRadians.vertices();
		return getPerpendicularCursor(vertices[_resizingEdgeIndex], vertices[(_resizingEdgeIndex + 1) % 4], _rotRect.yUp);
	}
	else
	{
		auto vertices = _rect.vertices();
		return getPerpendicularCursor(vertices[_resizingEdgeIndex], vertices[(_resizingEdgeIndex + 1) % 4], _rotRect.yUp);
	}

	return SDL_SYSTEM_CURSOR_SIZENESW;
}

void EditableObject::resize(const PointF& point)
{
	if (_rotRect.angle)
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		rotRectRadians.extendEdgeToPoint(point, _resizingEdgeIndex);
		_rotRect = rotRectRadians;
		_rotRect.angle = rad2deg(_rotRect.angle);
		_rect = _rotRect.toRect();
		setSize(_rect.size);
	}
	else
	{
		_rotRect.extendEdgeToPoint(point, _resizingEdgeIndex);
		_rect = _rotRect.toRect();
		setSize(_rect.size);
	}
}

void EditableObject::rotate(int angleDegrees) 
{ 
	_rotRect.angle = float( int(_rotRect.angle + angleDegrees) % 360 );
	_renderedName->setAngle(-_rotRect.angle);
	_renderedCategory->setAngle(-_rotRect.angle);
}

void EditableObject::draw(SDL_Renderer* renderer, Transform camera)
{
	if (!_visible)
		return;

	if (_rotRect.angle == 0)
	{
		RenderableObject::draw(renderer, camera);
		return;
	}

	RotatedRectF rotRectRadians = _rotRect;
	rotRectRadians.angle = deg2rad(_rotRect.angle);
	std::array < PointF, 4> drawVertices = rotRectRadians.vertices();
	for (int i = 0; i < 4; i++)
		drawVertices[i] = camera(drawVertices[i]);

	FillOBB(renderer, drawVertices, _color);

	if (_borderThickness)
		DrawThickOBB(renderer, drawVertices, _borderThickness, _borderColor);
	else
		DrawOBB(renderer, drawVertices, _borderColor);
}