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
// - implements Editor User Interface with:
//    - configurable helpbox (bottom region)
//    - scene coords at cursor's position
//    - cursor's system icon states
class agp::EditorUI : public UIScene
{
	protected:


		// scale parameters in [0,1] relative window coords
		static constexpr float CURSOR_TEXT_HEIGHT = 0.04f;
		static constexpr float CURSOR_TEXT_MARGIN_X = 0.02f;
		static constexpr float CURSOR_TEXT_MARGIN_Y = 0.02f;
		static constexpr float HELPBOX_TEXT_HEIGHT = 0.06f;
		static constexpr float HELPBOX_MARGIN_X = 0.01f;
		static constexpr float HELPBOX_MARGIN_Y = 0.01f;

		static constexpr int HELPBOX_MAX_ROWS = 3;

		RenderableObject* _cursorCoords;
		RenderableObject* _helpboxRows[HELPBOX_MAX_ROWS];
		SDL_Cursor* _cursor;

	public:

		EditorUI();
		virtual ~EditorUI();

		void setCursor(SDL_SystemCursor cursor);
		void setCursorText(const std::string& text, const Color& textColor = { 0,0,0 });
		void setHelpboxText(int row, const std::string& text, const Color& textColor = { 0,0,0 });
		void clearHelpboxText();

		virtual void update(float timeToSimulate) override;

		virtual void event(SDL_Event& evt) override;
};