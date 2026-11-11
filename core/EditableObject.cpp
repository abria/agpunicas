// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditableObject.h"
#include "LevelData.h"
#include "Scene.h"
#include "graphicsUtils.h"
#include "sdlUtils.h"
#include "Game.h"
#include "Window.h"
#include "Sprite.h"
#include "TextSprite.h"
#include "EditorStyle.h"

using namespace agp;

EditableObject::EditableObject(Scene* scene, const RectF& r, const std::string& name, int category, std::vector<std::string>& categories)
	: RenderableObject(scene, r, nullptr, 1), _categories(categories)
{
	_category = category;
	_name = name;
	_selected = false;
	_rotRect.center = rect().center();
	_rotRect.size = rect().size;
	_rotRect.angle = 0;
	_rotRect.yUp = rect().yUp;
	_resizingEdgeIndex = -1;

	init();
}

EditableObject::EditableObject(Scene* scene, const LineF& line, const std::string& name, int category, std::vector<std::string>& categories)
	: RenderableObject(scene, line.boundingRect(scene->rect().yUp), nullptr, 1), _categories(categories)
{
	_category = category;
	_name = name;
	_selected = false;
	_rotRect.center = rect().center();
	_rotRect.size = rect().size;
	_rotRect.angle = 0;
	_rotRect.yUp = rect().yUp;
	_resizingEdgeIndex = -1;
	_multiline.push_back(line.start);
	_multiline.push_back(line.end);

	init();
}

EditableObject::EditableObject(Scene* scene, const nlohmann::ordered_json& j, std::vector<std::string>& categories)
	: RenderableObject(scene, RectF(), nullptr, 1), _categories(categories)
{
	_json = j;
	_category = j.at("category");
	_name = j.value("name", "");
	_selected = false;
	_resizingEdgeIndex = -1;

	if (j.contains("multiline"))
	{
		_multiline = LevelData::multiline(j.at("multiline"));
		setRect(LineF(_multiline[0], _multiline[1]).boundingRect(_scene->rect().yUp));
		_rotRect = rect();
	}
	else
	{
		_rotRect = LevelData::rotRect(j);
		setRect(_rotRect.toRect());
		// editor rotations are stored in degrees
		_rotRect.angle = j.contains("rotRect") ? j.at("rotRect").at("angle").get<float>() : 0;
	}

	init();

	if (_multiline.size())
		updateLineRect();
}

nlohmann::ordered_json EditableObject::toJson()
{
	nlohmann::ordered_json j = _json.is_object() ? _json : nlohmann::ordered_json::object();
	// geometry may change type after editing
	j.erase("rect");
	j.erase("rotRect");
	j.erase("multiline");
	j.erase("layer"); // drawing order belongs to the game classes, not the editor
	j["category"] = _category;
	if (_name.empty())
		j.erase("name");
	else
		j["name"] = _name;

	if (_multiline.size())
	{
		std::vector <nlohmann::ordered_json> jsonObjects;
		for (auto& obj : _multiline)
		{
			nlohmann::ordered_json jline;
			jline["x"] = obj.x;
			jline["y"] = obj.y;
			jsonObjects.push_back(jline);
		}
		j["multiline"] = jsonObjects;
	}
	else if (_rotRect.angle)
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
		j["rect"]["x"] = rect().pos.x;
		j["rect"]["y"] = rect().pos.y;
		j["rect"]["width"] = rect().size.x;
		j["rect"]["height"] = rect().size.y;
		j["rect"]["yUp"] = rect().yUp;
	}

	return j;
}

void EditableObject::init()
{
	Color col = editorStyle::categoryColor(_category);
	setColor(col.adjustAlpha(ALPHA_NORMAL));
	setBorderThickness(NORMAL_BORDER_THICKNESS);
	setBorderColor(col);

	_renderedName = new RenderableObject(
		_scene,
		rect(),
		new TextSprite(_name, editorStyle::FONT, editorStyle::TEXT, { NAME_MARGIN_X, 0.0f }, { 0, NAME_MAX_HEIGHT },
			TextSprite::VAlign::BOTTOM, TextSprite::HAlign::CENTER, TextSprite::Style::NORMAL), 2);
	_renderedName->setAngle(-_rotRect.angle);
	_renderedName->setVisible(!_name.empty());

	_renderedCategory = new RenderableObject(
		_scene,
		rect(),
		new TextSprite(_categories[_category], editorStyle::FONT, col, {NAME_MARGIN_X, CATEGORY_MARGIN_Y}, { 0, CATEGORY_MAX_HEIGHT },
			TextSprite::VAlign::TOP, TextSprite::HAlign::CENTER, TextSprite::Style::NORMAL), 2);
	_renderedCategory->setAngle(-_rotRect.angle);
	dynamic_cast<TextSprite*>(_renderedName->sprite())->setBackgroundColor(editorStyle::PANEL);
	dynamic_cast<TextSprite*>(_renderedCategory->sprite())->setBackgroundColor(editorStyle::PANEL);
	updateLabelLayout();
}

void EditableObject::updateLabelLayout()
{
	// Reserve separate top/bottom bands, including on small one-cell objects.
	auto category = dynamic_cast<TextSprite*>(_renderedCategory->sprite());
	auto name = dynamic_cast<TextSprite*>(_renderedName->sprite());
	PointF margin{ std::min(float(NAME_MARGIN_X), rect().size.x * 0.05f),
		std::min(float(CATEGORY_MARGIN_Y), rect().size.y * 0.08f) };
	category->setMargin(margin);
	name->setMargin(margin);
	category->setMaxSize({ 0, std::min(float(CATEGORY_MAX_HEIGHT), rect().size.y * 0.28f) });
	name->setMaxSize({ 0, std::min(float(NAME_MAX_HEIGHT), rect().size.y * 0.38f) });
}

void EditableObject::kill()
{
	if (_killed)
		return;
	Object::kill();
	if (_renderedName)
		_renderedName->kill();
	if (_renderedCategory)
		_renderedCategory->kill();
}

void EditableObject::setCategory(int newCategory)
{ 
	_category = newCategory; 

	Color col = editorStyle::categoryColor(_category);
	setColor(col.adjustAlpha(_selected ? ALPHA_SELECTED : (_focused ? ALPHA_FOCUSED : ALPHA_NORMAL)));
	setBorderColor(col);

	dynamic_cast<TextSprite*>(_renderedCategory->sprite())->setText(_categories[_category]);
	dynamic_cast<TextSprite*>(_renderedCategory->sprite())->setColor(col);
	dynamic_cast<TextSprite*>(_renderedName->sprite())->setColor(editorStyle::TEXT);
}

void EditableObject::setName(const std::string& name)
{ 
	_name = name;
	dynamic_cast<TextSprite*>(_renderedName->sprite())->setText(_name);
	_renderedName->setVisible(_visible && !_name.empty());
}

void EditableObject::setFocused(bool on)
{
	_focused = on;
	if (_selected)
		return;
	_color.a = on ? ALPHA_FOCUSED : ALPHA_NORMAL;
	setBorderThickness(on ? FOCUSED_BORDER_THICKNESS : NORMAL_BORDER_THICKNESS);
}

void EditableObject::setSelected(bool on)
{
	_selected = on;
	_color.a = on ? ALPHA_SELECTED : (_focused ? ALPHA_FOCUSED : ALPHA_NORMAL);
	setBorderThickness(on ? SELECTED_BORDER_THICKNESS :
		(_focused ? FOCUSED_BORDER_THICKNESS : NORMAL_BORDER_THICKNESS));
}

bool EditableObject::contains(const Vec2Df& p)
{
	if (_multiline.size())
	{
		for (int i = 0; i < _multiline.size() - 1; i++)
			if (LineF(_multiline[i], _multiline[i + 1]).distance(p) < LINE_HOOK_DISTANCE)
				return true;
		return false;
	}
	else if (_rotRect.angle == 0)
		return RenderableObject::contains(p);
	else
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		return rotRectRadians.contains(p);
	}
}

bool EditableObject::intersectsRectShallow(const RectF& r)
{
	if (_multiline.size())
	{
		for (int i = 0; i < _multiline.size() - 1; i++)
			if (r.contains(_multiline[i]))
				return true;
		return false;
	}
	else if (_rotRect.angle == 0)
		return RenderableObject::intersectsRectShallow(r);
	else
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		return rotRectRadians.boundingRect().intersects(r);
	}
}

void EditableObject::setVisible(bool visible)
{
	RenderableObject::setVisible(visible);
	_renderedName->setVisible(visible && !_name.empty());
	_renderedCategory->setVisible(visible);
}

void EditableObject::setPos(const PointF& newPos)
{
	if (_multiline.size())
		return;

	RenderableObject::setPos(newPos);
	_renderedName->setPos(newPos);
	_renderedCategory->setPos(newPos);
	_rotRect.center = rect().center();
}

void EditableObject::setSize(const PointF& newSize)
{ 
	if (newSize.x <= 0 || newSize.y <= 0)
		return;

	RenderableObject::setSize(newSize);
	_renderedName->setRect(rect());
	_renderedCategory->setRect(rect());
	updateLabelLayout();

	_rotRect.center = rect().center();
	_rotRect.size = rect().size;
}

bool EditableObject::resizableAt(const PointF& point)
{
	if (_multiline.size())
		return false;

	float minDist = inf<float>();
	if (_rotRect.angle)
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		_resizingEdgeIndex = closestEdgeIndex(rotRectRadians.vertices(), point, minDist);
	}
	else
		_resizingEdgeIndex = closestEdgeIndex(rect().vertices(), point, minDist);

	return minDist < RESIZING_HOOK_DISTANCE;
}

SDL_SystemCursor EditableObject::resizeCursor()
{
	if (_multiline.size())
		return SDL_SYSTEM_CURSOR_DEFAULT;

	if (_rotRect.angle)
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		auto vertices = rotRectRadians.vertices();
		return getPerpendicularCursor(vertices[_resizingEdgeIndex], vertices[(_resizingEdgeIndex + 1) % 4], _rotRect.yUp);
	}
	else
	{
		auto vertices = rect().vertices();
		return getPerpendicularCursor(vertices[_resizingEdgeIndex], vertices[(_resizingEdgeIndex + 1) % 4], _rotRect.yUp);
	}

	return SDL_SYSTEM_CURSOR_NESW_RESIZE;
}

void EditableObject::resize(const PointF& point)
{
	if (_multiline.size())
		return;

	if (_rotRect.angle)
	{
		RotatedRectF rotRectRadians = _rotRect;
		rotRectRadians.angle = deg2rad(_rotRect.angle);
		rotRectRadians.extendEdgeToPoint(point, _resizingEdgeIndex);
		_rotRect = rotRectRadians;
		_rotRect.angle = rad2deg(_rotRect.angle);
		setRect(_rotRect.toRect());
		setSize(rect().size);
	}
	else
	{
		_rotRect.extendEdgeToPoint(point, _resizingEdgeIndex);
		setRect(_rotRect.toRect());
		setSize(rect().size);
	}
}

void EditableObject::rotate(int angleDegrees) 
{ 
	if (_multiline.size())
		return;

	_rotRect.angle = float( int(_rotRect.angle + angleDegrees) % 360 );
	_renderedName->setAngle(-_rotRect.angle);
	_renderedCategory->setAngle(-_rotRect.angle);
}

void EditableObject::addLinePoint(const PointF& p)
{
	_multiline.push_back(p);
}

void EditableObject::replaceLastPoint(const PointF& p)
{
	if (_multiline.size())
	{
		_multiline.pop_back();
		_multiline.push_back(p);

		if (_multiline.size() == 2)
			updateLineRect();
	}
}

void EditableObject::updateLineRect()
{
	LineF firstLine = LineF(_multiline[0], _multiline[1]);
	_rotRect.center = firstLine.boundingRect(_rotRect.yUp).center();
	_rotRect.size = { (firstLine.end - firstLine.start).mag(), 1.5f };
	_rotRect.angle = (firstLine.end - firstLine.start).angle(_rotRect.yUp);
	_rotRect.angle = rad2deg(_rotRect.angle);
	_renderedName->setAngle(-_rotRect.angle);
	_renderedCategory->setAngle(-_rotRect.angle);
	setRect(_rotRect.toRect());
	setSize(rect().size);
}

void EditableObject::undoLineLastPoint()
{
	if (_multiline.size() > 2)
		_multiline.pop_back();
}

void EditableObject::draw(SDL_Renderer* renderer, Transform camera)
{
	if (!_visible)
		return;

	if (_multiline.empty())
	{
		RotatedRectF rotated = _rotRect;
		rotated.angle = deg2rad(rotated.angle);
		auto vertices = rotated.vertices();
		for (auto& vertex : vertices)
			vertex = camera(vertex);
		if ((vertices[1] - vertices[0]).mag() == 0 || (vertices[2] - vertices[1]).mag() == 0)
			return;

		FillOBB(renderer, vertices, _color);
		// Opaque dark keyline + bright category stroke work on light and dark scenes.
		DrawThickOBB(renderer, vertices, _borderThickness + (_selected ? 8 : 4), editorStyle::PANEL);
		if (_selected)
			DrawThickOBB(renderer, vertices, _borderThickness + 4, editorStyle::TEXT);
		DrawThickOBB(renderer, vertices, _borderThickness, _borderColor);

		if (_selected)
			for (const auto& vertex : vertices)
			{
				FillOBB(renderer, RectF(vertex.x - 4, vertex.y - 4, 8, 8).vertices(), editorStyle::PANEL);
				FillOBB(renderer, RectF(vertex.x - 2, vertex.y - 2, 4, 4).vertices(), editorStyle::TEXT);
			}
	}
	else
	{
		for (size_t i = 1; i < _multiline.size(); ++i)
		{
			PointF start = camera(_multiline[i - 1]), end = camera(_multiline[i]);
			PointF delta = end - start;
			float length = delta.mag();
			if (length <= 0)
				continue;
			float width = _borderThickness + 1;
			DrawThickLine(renderer, start, end, width + (_selected ? 8 : 4), editorStyle::PANEL);
			if (_selected)
				DrawThickLine(renderer, start, end, width + 4, editorStyle::TEXT);
			DrawThickLine(renderer, start, end, width, _borderColor);
		}
	}
}
