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

		int _category;

	public:

		EditableObject(Scene* scene, const RectF& rect, int category, int layer = 0);
		EditableObject(Scene* scene, const std::string& fromStr);

		int category() { return _category; };

		virtual std::string name() override {
			return strprintf("EditableObject[%d]", _id);
		}

		std::string serialize();
		void unserialize(const std::string& str);
};