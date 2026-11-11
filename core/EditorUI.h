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
		static constexpr int HELPBOX_MAX_ROWS = 3;

		RenderableObject* _cursorCoords;
		RenderableObject* _helpboxPanel;
		RenderableObject* _helpboxAccent;
		std::vector<RenderableObject*> _helpboxLines;
		std::vector<Color> _helpboxLineColors;
		std::string _helpboxText[HELPBOX_MAX_ROWS];
		Color _helpboxColors[HELPBOX_MAX_ROWS];
		SDL_Cursor* _cursor;
		bool _editing;
		bool _autohide;
		bool _helpboxDirty;
		bool _helpboxHovered;
		float _helpboxOpacity;
		float _helpboxHideDelay;

		void layoutHelpbox();
		void applyHelpboxOpacity();

	public:

		EditorUI();
		virtual ~EditorUI();

		void setEditing(bool active) { _editing = active; if (active) _helpboxHideDelay = 2; }
		void setCursor(SDL_SystemCursor cursor);
		void setCursorText(const std::string& text, const Color& textColor = { 0,0,0,0 });
		void setHelpboxText(int row, const std::string& text, const Color& textColor = { 0,0,0,0 });
		void clearHelpboxText();
		void toggleAutohide();

		virtual void render() override;
		virtual void update(float timeToSimulate) override;

		virtual void event(SDL_Event& evt) override;
};