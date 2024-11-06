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

using namespace agp;

DialogBox::DialogBox(const std::string& text, int visibleLines, int wrapLength)
	: UIScene(RectF(0, 0, 256, 224), { 1, 1 })
{
	_text = text;
	_visibleLines = visibleLines;
	_blocking = true;
	_backgroundColor = { 0, 0, 255, 128 };
	
	std::vector<std::string> textRows = wrapText(_text, wrapLength);
	for (int i = 0; i < textRows.size(); i++)
	{
		_charObjects.push_back(std::vector <RenderableObject*>());
		int x = 0;
		for (int j = 0; j < textRows[i].size(); j++)
		{
			Sprite* charSprite = SpriteFactory::instance()->getChar(textRows[i][j]);
			int dx = charSprite ? charSprite->rect().size.x : 7;
			_charObjects.back().push_back(new RenderableObject(this, RectI(x, 18 * i, dx, 15), charSprite, 1));
			x += dx;
		}
	}

	// SNES aspect ratio
	_view = new View(this, _rect);
	_view->setFixedAspectRatio(256.0f/224);
	_view->setRect(RectF(0, 0, 256, 224));
	//_view->setClipRect(RectF(0, 0, 1.0f, 51.0f/224));
	new Clipper(this, RectF(0, 0, 1.0f, 51.0f / 224), 0);
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
			Game::instance()->popScene();
	}
}