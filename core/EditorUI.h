// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "UIScene.h"
#include "RenderableObject.h"
#include "geometryUtils.h"

namespace agp
{
	class EditorUI;
}

// EditorUI
// - implements Editor User Interface (commands, buttons, etc.)
class agp::EditorUI : public UIScene
{
	protected:

		RenderableObject* _cursorObj;
		static const int MAX_HELP_ROWS = 3;
		RenderableObject* _helpObjects[MAX_HELP_ROWS];
		SDL_Cursor* _crossCursor;

		const float cursorTextSize = 0.03f;	// in [0,1] relative window coords
		const float helpTextSize = 0.05f;	// in [0,1] relative window coords

	public:

		EditorUI();
		virtual ~EditorUI();

		void setCrossCursor(bool on);
		void setCursorText(const std::string& text, const Color& textColor = { 0,0,0 });
		void setHelpText(int row, const std::string& text, const Color& textColor = { 0,0,0 });
		void clearHelpText();

		virtual void update(float timeToSimulate) override;

		virtual void event(SDL_Event& evt) override;
};