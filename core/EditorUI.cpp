// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorUI.h"
#include "EditorScene.h"
#include "View.h"
#include "Game.h"
#include "TextSprite.h"

using namespace agp;

EditorUI::EditorUI()
	: UIScene(RectF(0, 0, 1, 1), {1,1})
{
	_cursorObj = new RenderableObject(this, RectF(0, 0, 1, cursorTextSize), 
		new TextSprite("0,0", "font.ttf"));

	for (int i = 0; i < MAX_HELP_ROWS; i++)
	{
		_helpObjects[i] = new RenderableObject(this, RectF(0, 1 - helpTextSize * (MAX_HELP_ROWS - i), 1, helpTextSize),
			new TextSprite("", "font.ttf", { 0,0,0 }, { 0.01f, 0.01f }));
		_helpObjects[i]->setBackgroundColor(Color(255, 255, 255, 128));
		_helpObjects[i]->setVisible(false);
	}

	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, 0, 1, 1));

	_crossCursor = nullptr;
}

EditorUI::~EditorUI()
{
	setCrossCursor(false);
}

void EditorUI::setCrossCursor(bool on)
{
	if (on && _crossCursor)
		return;
	if (!on && !_crossCursor)
		return;

	if(on)
		_crossCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
	else
	{
		SDL_FreeCursor(_crossCursor);
		_crossCursor = nullptr;
	}
	SDL_SetCursor(_crossCursor);
}

void EditorUI::setCursorText(const std::string& text, const Color& textColor)
{
	dynamic_cast<TextSprite*>(_cursorObj->sprite())->setText(text);
	dynamic_cast<TextSprite*>(_cursorObj->sprite())->setColor(textColor);
}

void EditorUI::setHelpText(int row, const std::string& text, const Color& textColor)
{
	_helpObjects[MAX_HELP_ROWS - row - 1]->setVisible(true);
	dynamic_cast<TextSprite*>(_helpObjects[MAX_HELP_ROWS - row - 1]->sprite())->setText(text);
	dynamic_cast<TextSprite*>(_helpObjects[MAX_HELP_ROWS - row - 1]->sprite())->setColor(textColor);
}

void EditorUI::clearHelpText()
{
	for (int i = 0; i < MAX_HELP_ROWS; i++)
		_helpObjects[i]->setVisible(false);
}

void EditorUI::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	if (!_active)
		return;
}

void EditorUI::event(SDL_Event& evt)
{
	UIScene::event(evt);

	if (evt.type == SDL_MOUSEMOTION)
	{
		PointF mousePoint(float(evt.button.x), float(evt.button.y));
		mousePoint = _view->mapToScene(mousePoint);

		_cursorObj->setPos(mousePoint + PointF{ 0.02f, 0.02f });
	}
}