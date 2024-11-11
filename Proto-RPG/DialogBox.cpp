// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "DialogBox.h"
#include "SpriteFactory.h"
#include "View.h"
#include "Game.h"
#include "stringUtils.h"
#include "Clipper.h"
#include "RPGGame.h"

using namespace agp;

DialogBox::DialogBox(
	const std::string& text,
	const std::vector<Option>& options,
	const PointF& pos, 
	int visibleLines, 
	int wrapLength)
	: UIScene(RectF(0, 0, 256, 224), { 1, 1 })
{
	_text = text;
	_visibleLines = visibleLines;
	_blocking = true;
	_charIterator = 0;
	_rowIterator = 0;
	_scrollingRow = false;
	_currentOption = 0;
	_options = options;

	// place here a more complex background (e.g. sprite) if needed
	RenderableObject* background = new RenderableObject(this, RectF(pos.x, pos.y, float(_fontSize.x * wrapLength), float(_fontSize.y + _rowMargin) * visibleLines), Color(0, 0, 255, 128), 0);
	background->setBorderColor(Color(0, 0, 0));

	// wrap text
	std::vector<std::string> textRows = wrapText(_text, wrapLength);

	// add options
	std::string optionsRow;
	for(int i=0; i< _options.size(); i++)
		optionsRow += " >" + _options[i].name + " ";
	textRows.push_back(optionsRow);

	// text to sprites
	for (int i = 0; i < textRows.size(); i++)
	{
		_chars.push_back(std::vector <RenderableObject*>());
		float x = 0;
		for (int j = 0; j < textRows[i].size(); j++)
		{
			Sprite* charSprite = SpriteFactory::instance()->getChar(textRows[i][j]);
			float dx = charSprite ? float(charSprite->rect().size.x) : float(_fontSize.x);
			_chars.back().push_back(new RenderableObject(this, RectF(pos.x + x, pos.y + float(_fontSize.y + _rowMargin) * i, dx, float(_fontSize.y)), charSprite, 1));
			_chars.back().back()->setVisible(options.size() && i == textRows.size() - 1);
			x += dx;
			if (textRows[i][j] == '>')
			{
				_arrows.push_back(_chars.back().back());
				_chars.back().back()->setVisible(false);
			}
		}
	}

	// select default option, if options are available
	updateCurrentOption();

	// SNES aspect ratio
	_view = new View(this, _rect);
	_view->setFixedAspectRatio(256.0f/224);
	_view->setRect(RectF(0, 0, 256, 224));
	new Clipper(this, RectF(0, background->pos().y/224, 1.0f, background->rect().size.y / 224), 0);
}

void DialogBox::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	if (!_active)
		return;

	bool faster = false;
	const Uint8* keyboard = SDL_GetKeyboardState(0);
	if (keyboard[SDL_SCANCODE_DOWN])
		faster = true;

	// advance one-by-one character display
	// and activate scrolling on newline
	if (!_scrollingRow && _rowIterator < _chars.size())
	{
		_charIterator += (faster ? _fasterScrollMult : 1) * _charsPerSecond * timeToSimulate;
		if (_charIterator >= _chars[_rowIterator].size())
		{
			_charIterator = 0;
			_rowIterator++;
			if (_rowIterator >= _visibleLines && _rowIterator < _chars.size())
			{
				_scrollingRow = true;
				schedule("scrolling_off", _scrollingRowTime, [this]() {_scrollingRow = false; });
			}
		}
		if (!_scrollingRow && _rowIterator < _chars.size() && !(_options.size() && _rowIterator == _chars.size() - 1))
			_chars[_rowIterator][int(_charIterator)]->setVisible(true);
	}

	// scrolling animation
	if (_scrollingRow)
		for (int i = 0; i < _chars.size(); i++)
			for (int j = 0; j < _chars[i].size(); j++)
				_chars[i][j]->setPos(_chars[i][j]->pos() + PointF{ 0, -((_fontSize.y + _rowMargin) / _scrollingRowTime) * timeToSimulate });
}

void DialogBox::event(SDL_Event& evt)
{
	UIScene::event(evt);

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.scancode == SDL_SCANCODE_RIGHT && _options.size())
		{
			_currentOption = (_currentOption + 1) % _options.size();
			updateCurrentOption();
		}
		else if (evt.key.keysym.scancode == SDL_SCANCODE_RETURN && _rowIterator == _chars.size())
		{
			if (_options.size())
				_options[_currentOption].action();

			Game::instance()->popScene();
		}
	}
}

void DialogBox::updateCurrentOption()
{
	for (int i = 0; i < _arrows.size(); i++)
		_arrows[i]->setVisible(i == _currentOption);
}