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
	const std::string& name,
	const std::string& text,
	const std::string& options,
	const PointF& pos, 
	int visibleLines, 
	int wrapLength)
	: UIScene(RectF(0, 0, 256, 224), { 1, 1 })
{
	_name = name;
	_text = text;
	_visibleLines = visibleLines;
	_blocking = true;
	_charIterator = 0;
	_rowIterator = 0;
	_scrollingRow = false;
	_currentOption = 0;

	// place here a more complex background (e.g. sprite) if needed
	RenderableObject* background = new RenderableObject(this, RectF(pos.x, pos.y, float(_fontSize.x * wrapLength), float(_fontSize.y + _rowMargin) * visibleLines), Color(0, 0, 255, 128), 0);
	background->setBorderColor(Color(0, 0, 0));

	// wrap text
	std::vector<std::string> textRows = wrapText(_text, wrapLength);

	// add options
	if (options.size())
		split(options, ",", _optionList);
	std::string optionsRow;
	for(int i=0; i< _optionList.size(); i++)
		optionsRow += " >" + _optionList[i] + " ";
	textRows.push_back(optionsRow);

	// text to sprites
	for (int i = 0; i < textRows.size(); i++)
	{
		_charObjects.push_back(std::vector <RenderableObject*>());
		float x = 0;
		for (int j = 0; j < textRows[i].size(); j++)
		{
			Sprite* charSprite = SpriteFactory::instance()->getChar(textRows[i][j]);
			float dx = charSprite ? float(charSprite->rect().size.x) : float(_fontSize.x);
			_charObjects.back().push_back(new RenderableObject(this, RectF(pos.x + x, pos.y + float(_fontSize.y + _rowMargin) * i, dx, float(_fontSize.y)), charSprite, 1));
			_charObjects.back().back()->setVisible(options.size() && i == textRows.size() - 1);
			x += dx;
			if (textRows[i][j] == '>')
			{
				_optionObjects.push_back(_charObjects.back().back());
				_charObjects.back().back()->setVisible(false);
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

	if (!_scrollingRow && _rowIterator < _charObjects.size())
	{
		_charIterator += (faster ? _fasterScrollMult : 1) * _charsPerSecond * timeToSimulate;
		if (_charIterator >= _charObjects[_rowIterator].size())
		{
			_charIterator = 0;
			_rowIterator++;
			if (_rowIterator >= _visibleLines && _rowIterator < _charObjects.size())
			{
				_scrollingRow = true;
				schedule("scrolling_off", _scrollingRowTime, [this]() {_scrollingRow = false; });
			}
		}
		if (!_scrollingRow && _rowIterator < _charObjects.size() && !(_optionObjects.size() && _rowIterator == _charObjects.size() - 1))
			_charObjects[_rowIterator][int(_charIterator)]->setVisible(true);
	}

	if (_scrollingRow)
		for (int i = 0; i < _charObjects.size(); i++)
			for (int j = 0; j < _charObjects[i].size(); j++)
				_charObjects[i][j]->setPos(_charObjects[i][j]->pos() + PointF{ 0, -((_fontSize.y + _rowMargin) / _scrollingRowTime) * timeToSimulate });
}

void DialogBox::event(SDL_Event& evt)
{
	UIScene::event(evt);

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.scancode == SDL_SCANCODE_RIGHT && _optionObjects.size())
		{
			_currentOption = (_currentOption + 1) % _optionObjects.size();
			updateCurrentOption();
		}
		else if (evt.key.keysym.scancode == SDL_SCANCODE_RETURN)
		{
			if (_optionObjects.size())
				dynamic_cast<RPGGame*>(Game::instance())->dialogOptionEntered(_name, _optionList[_currentOption]);

			Game::instance()->popScene();
		}
	}
}

void DialogBox::updateCurrentOption()
{
	for (int i = 0; i < _optionObjects.size(); i++)
		_optionObjects[i]->setVisible(i == _currentOption);
}