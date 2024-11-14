// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "RenderableObject.h"
#include "json.hpp"

namespace agp
{
	class Scene;
	class EditableObject;
}

// EditableObject class.
// - serializable editable object for level editing
class agp::EditableObject : public RenderableObject
{
	protected:

		std::string _text;
		int _category;

		void updateColor();

	public:

		EditableObject(Scene* scene, const RectF& rect, int category);
		EditableObject(Scene* scene, const nlohmann::json& fromJson);

		int category() { return _category; }
		void setCategory(int newCategory);

		virtual std::string name() override { return strprintf("EditableObject[%d]", _id); }

		nlohmann::ordered_json toJson();
};