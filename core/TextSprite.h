// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Sprite.h"
#include "graphicsUtils.h"

namespace agp
{
	class TextSprite;
}

// TextSprite
// - sprite that renders font-based left-justified text
class agp::TextSprite : public Sprite
{
	public:

		enum class VAlign { CENTER, TOP, BOTTOM };
		enum class HAlign { LEFT, CENTER, RIGHT };
		enum class Style {NORMAL, BOLD, ITALIC, UNDERLINE};

	protected:

		std::string _text;
		Color _fontColor;
		std::string _fontPath;
		Style _fontStyle;
		VAlign _vertAlign;
		HAlign _horzAlign;
		PointF _margin;						// in scene units
		bool _regenerateTexture;

		static const int pointSize = 100;	// font resolution when generated natively

	public:

		TextSprite(
			const std::string& text,
			const std::string& fontPath,
			const Color& fontColor = { 0,0,0 },
			const PointF& margin = { 0,0 },
			Direction expandDir = Direction::RIGHT,
			VAlign vertAlign = VAlign::CENTER,
			HAlign horzAlign = HAlign::LEFT,
			Style style = Style::NORMAL);
		virtual ~TextSprite();

		// setters
		void setText(const std::string& newText);
		void setColor(const Color& newColor);
		void setMargin(const PointF& newMargin);

		// extends render method (+regenerate texture if needed, +align)
		virtual void render(
			SDL_Renderer* renderer,
			const RectF& drawRect,
			Transform camera,
			const Point& pixelUnitSize,
			float angle = 0,
			SDL_RendererFlip flip = SDL_FLIP_NONE) override;
};