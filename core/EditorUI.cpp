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
	: UIScene(RectF(0, 0, 1, 1 / Game::instance()->aspectRatio()), { 1,1 })
{
	_cursorCoords = new RenderableObject(this, RectF(0, 0, 1, CURSOR_TEXT_HEIGHT * _rect.size.y),
		new TextSprite("", "Lucida"), 1);

	float helpboxTextHeighNorm = HELPBOX_TEXT_HEIGHT * _rect.size.y;
	float helpboxMarginXNorm = HELPBOX_MARGIN_X * _rect.size.x;
	float helpboxMarginYNorm = HELPBOX_MARGIN_Y * _rect.size.y;
	for (int i = 0; i < HELPBOX_MAX_ROWS; i++)
	{
		_helpboxRows[i] = new RenderableObject(this, RectF(0, _rect.bottom() - helpboxTextHeighNorm * (HELPBOX_MAX_ROWS - i), 1, helpboxTextHeighNorm),
			new TextSprite("", "Lucida", { 0,0,0,0 }, { helpboxMarginXNorm, helpboxMarginYNorm }));
		_helpboxRows[i]->setBackgroundColor(Color(255, 255, 255, HELPBOX_ALPHA));
		_helpboxRows[i]->setVisible(false);
	}

	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(_rect);
	_showing = false;
	_hiding = false;
	_editing = false;
	_autohide = false;

	_cursor = nullptr;
}

EditorUI::~EditorUI()
{
	SDL_FreeCursor(_cursor);
	_cursor = nullptr;
}

void EditorUI::setCursor(SDL_SystemCursor cursor)
{
	if (_cursor)
		SDL_FreeCursor(_cursor);

	_cursor = SDL_CreateSystemCursor(cursor);
	SDL_SetCursor(_cursor);
}

void EditorUI::setCursorText(const std::string& text, const Color& textColor)
{
	dynamic_cast<TextSprite*>(_cursorCoords->sprite())->setText(text);
	if(textColor.a)
		dynamic_cast<TextSprite*>(_cursorCoords->sprite())->setColor(textColor);
}

void EditorUI::setHelpboxText(int row, const std::string& text, const Color& textColor)
{
	_helpboxRows[HELPBOX_MAX_ROWS - row - 1]->setVisible(true);
	dynamic_cast<TextSprite*>(_helpboxRows[HELPBOX_MAX_ROWS - row - 1]->sprite())->setText(text);
	if(textColor.a)
		dynamic_cast<TextSprite*>(_helpboxRows[HELPBOX_MAX_ROWS - row - 1]->sprite())->setColor(textColor);
	_showing = true;
	_hiding = false;
}

void EditorUI::clearHelpboxText()
{
	for (int i = 0; i < HELPBOX_MAX_ROWS; i++)
		_helpboxRows[i]->setVisible(false);
}

void EditorUI::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	if (!_active)
		return;

	if (!_autohide)
		return;

	bool showingEnded = false;
	bool hidingEnded = false;
	for (int i = 0; i < HELPBOX_MAX_ROWS; i++)
	{
		TextSprite* sprite = dynamic_cast<TextSprite*>(_helpboxRows[i]->sprite());
		if (_showing)
		{
			_helpboxRows[i]->setBackgroundColor(_helpboxRows[i]->backgroundColor().adjustAlpha(_helpboxRows[i]->backgroundColor().a + 2));
			sprite->setColor(sprite->color().adjustAlpha(sprite->color().a + 4));
			if(_helpboxRows[i]->backgroundColor().a > HELPBOX_ALPHA)
				_helpboxRows[i]->setBackgroundColor(_helpboxRows[i]->backgroundColor().adjustAlpha(HELPBOX_ALPHA));
			showingEnded = _helpboxRows[i]->backgroundColor().a >= HELPBOX_ALPHA && sprite->color().a == 255;
		}
		else if (_hiding)
		{
			_helpboxRows[i]->setBackgroundColor(_helpboxRows[i]->backgroundColor().adjustAlpha(_helpboxRows[i]->backgroundColor().a - 1));
			sprite->setColor(sprite->color().adjustAlpha(std::max(int(sprite->color().a) - 2, 1)));
			hidingEnded = _helpboxRows[i]->backgroundColor().a == 0 && sprite->color().a == 1;
		}
	}

	if (showingEnded)
		_showing = false;
	if (hidingEnded)
		_hiding = false;
}

void EditorUI::toggleAutohide() 
{ 
	_autohide = !_autohide; 
	if (!_autohide) 
	{ 
		_showing = false;
		_hiding = false;
		for (int i = 0; i < HELPBOX_MAX_ROWS; i++)
		{
			_helpboxRows[i]->setBackgroundColor(_helpboxRows[i]->backgroundColor().adjustAlpha(HELPBOX_ALPHA));
			TextSprite* sprite = dynamic_cast<TextSprite*>(_helpboxRows[i]->sprite());
			sprite->setColor(sprite->color().adjustAlpha(255));
		}
	} 
}

void EditorUI::event(SDL_Event& evt)
{
	UIScene::event(evt);

	if (evt.type == SDL_MOUSEMOTION)
	{
		PointF _mousePointCurr(float(evt.button.x), float(evt.button.y));
		_mousePointCurr = _view->mapToScene(_mousePointCurr);

		// helpbox show/hide
		if (_autohide && std::abs(_mousePointCurr.y - _rect.bottom()) < 0.1f)
		{
			_showing = true;
			_hiding = false;
		}
		else if(_autohide && !_editing && !_hiding)
		{
			schedule("hide_later", 1, [this]()
				{
					_showing = false;
					_hiding = true;
				}, 0, false);
		}

		_cursorCoords->setPos(_mousePointCurr + PointF{ CURSOR_TEXT_MARGIN_X * _rect.size.x, CURSOR_TEXT_MARGIN_Y * _rect.size.y });
	}
}