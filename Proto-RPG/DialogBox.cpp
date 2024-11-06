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

using namespace agp;

DialogBox::DialogBox(const std::string& text, int visibleLines, int wrapLength)
	: UIScene(RectF(0, 0, 256, 224), { 1, 1 })
{
	_text = text;
	_visibleLines = visibleLines;
	_blocking = true;
	
	std::vector<std::string> textRows = wrapText(_text, wrapLength);
	for (int i = 0; i < textRows.size(); i++)
	{
		_charObjects.push_back(std::vector <RenderableObject*>());
		for (int j = 0; j < textRows[i].size(); j++)
			_charObjects.back().push_back(new RenderableObject(this, RectI(6*j, 18*i, 6, 14), SpriteFactory::instance()->getChar(textRows[i][j]), 1));
	}

	// SNES aspect ratio
	_view = new View(this, _rect);
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
	_view->setRect(RectF(0, 0, 256, 224));
}

void DialogBox::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	if (!_active)
		return;
}

void DialogBox::event(SDL_Event& evt)
{
	UIScene::event(evt);

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.scancode == SDL_SCANCODE_RETURN)
			;
	}
}