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

		std::string _name;
		int _category;
		bool _selected;
		RenderableObject* _renderedText;
		RenderableObject* _renderedCategory;
		std::vector<std::string>& _categories;

		void init();

	public:

		EditableObject(Scene* scene, const RectF& rect, const std::string& name, int category, std::vector<std::string>& categories);
		EditableObject(Scene* scene, const nlohmann::json& fromJson, std::vector<std::string>& categories);
		virtual ~EditableObject();

		int category() { return _category; }
		void setCategory(int newCategory);
		std::string editName() { return _name; }
		void setName(const std::string& name);
		void setFocused(bool on);
		void setSelected(bool on);

		virtual void setVisible(bool visible) override;
		virtual void setPos(const PointF& newPos) override;
		virtual void setSize(const PointF& newSize) override;

		virtual std::string name() override { return strprintf("EditableObject[%d]", _id); }

		nlohmann::ordered_json toJson();
};