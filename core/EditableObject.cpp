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
#include "json.hpp"

using namespace agp;

EditableObject::EditableObject(Scene* scene, const RectF& rect, int category, int layer)
	: RenderableObject(scene, rect, nullptr, layer)
{
	_category = category;

	Color col = distinctColors(10)[_category];
	setColor(col.adjustAlpha(128));
	setBorderColor(col);
}

EditableObject::EditableObject(Scene* scene, const std::string& fromStr)
	: RenderableObject(scene, RectF(), nullptr, 100)
{
	unserialize(fromStr);

	Color col = distinctColors(10)[_category];
	setColor(col.adjustAlpha(128));
	setBorderColor(col);
}

std::string EditableObject::serialize()
{
	nlohmann::json jroot;
	jroot["category"] = _category;
	
	nlohmann::json jrect;
	jrect["x"] = _rect.pos.x;
	jrect["y"] = _rect.pos.y;
	jrect["width"] = _rect.size.x;
	jrect["height"] = _rect.size.y;
	jrect["yUp"] = _rect.yUp;
	jroot["rect"] = jrect;

	return jroot.dump();
}

void EditableObject::unserialize(const std::string& str)
{
	nlohmann::json jroot = nlohmann::json::parse(str);
	_category = jroot["category"];
	_rect.pos.x = jroot["rect"]["x"];
	_rect.pos.y = jroot["rect"]["y"];
	_rect.size.x = jroot["rect"]["width"];
	_rect.size.y = jroot["rect"]["height"];
	_rect.yUp = jroot["rect"]["yUp"];
}