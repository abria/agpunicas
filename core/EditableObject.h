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
#include "mathUtils.h"

namespace agp
{
	class Scene;
	class EditableObject;
}

// EditableObject class.
// - serializable editable object for level editing
// - supported geometries:
//   - AABB rect
//   - rotated rect
//   - multiline
class agp::EditableObject : public RenderableObject
{
	protected:

		std::string _name;
		int _category;
		bool _selected;
		RenderableObject* _renderedName;
		RenderableObject* _renderedCategory;
		std::vector<std::string>& _categories;
		RotatedRectF _rotRect;
		int _resizingEdgeIndex;
		std::vector<PointF> _multiline;

		// in scene coords
		static constexpr float CATEGORY_MAX_HEIGHT = 0.5f;
		static constexpr float CATEGORY_MARGIN_Y = 0.1f;
		static constexpr float NAME_MAX_HEIGHT = 1;
		static constexpr float NAME_MARGIN_X = 0.1f;
		static constexpr float RESIZING_HOOK_DISTANCE = 0.25f;
		static constexpr float LINE_THICKNESS = 0.1f;
		static constexpr float LINE_HOOK_DISTANCE = 0.3f;

		// in screen points
		static constexpr float SELECTED_BORDER_THICKNESS = 5.0f;

		// in alpha [0, 255] values
		static constexpr int ALPHA_NORMAL = 160;
		static constexpr int ALPHA_SELECTED = 100;
		static constexpr int ALPHA_FOCUSED = 200;

		void init();
		void updateLineRect();

	public:

		EditableObject(Scene* scene, const RectF& rect, const std::string& name, int category, std::vector<std::string>& categories);
		EditableObject(Scene* scene, const LineF& line, const std::string& name, int category, std::vector<std::string>& categories);
		EditableObject(Scene* scene, const nlohmann::json& fromJson, std::vector<std::string>& categories);
		virtual ~EditableObject();

		int category() const { return _category; }
		virtual void setCategory(int newCategory);
		std::string editName() const { return _name; }
		virtual void setName(const std::string& name);
		virtual void setFocused(bool on);
		virtual void setSelected(bool on);
		virtual void addLinePoint(const PointF& p);
		virtual void replaceLastPoint(const PointF& p);
		virtual void undoLineLastPoint();
		virtual bool isLine() { return _multiline.size(); }

		virtual bool resizableAt(const PointF& point);
		virtual void resize(const PointF& point);
		virtual void rotate(int angleDegrees);

		virtual SDL_SystemCursor resizeCursor();

		virtual bool shallowIntersects(const RectF& r) override;
		virtual bool contains(const Vec2Df& p) override;
		virtual void setVisible(bool visible) override;
		virtual void setPos(const PointF& newPos) override;
		virtual void setSize(const PointF& newSize) override;

		virtual void draw(SDL_Renderer* renderer, Transform camera) override;

		virtual std::string name() override { return strprintf("EditableObject[%d]", _id); }

		virtual nlohmann::ordered_json toJson();
};