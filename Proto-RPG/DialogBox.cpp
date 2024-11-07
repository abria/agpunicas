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

DialogBox::DialogBox(const std::string& text, const PointF& pos, int visibleLines, int wrapLength)
	: UIScene(RectF(0, 0, 256, 224), { 1, 1 })
{
	_text = text;
	_visibleLines = visibleLines;
	_blocking = true;

	RenderableObject* background = new RenderableObject(this, RectF(pos.x, pos.y, 7.0f * wrapLength, 18.0f * visibleLines), Color(0, 0, 255, 128), 0);
	
	std::vector<std::string> textRows = wrapText(_text, wrapLength);
	for (int i = 0; i < textRows.size(); i++)
	{
		_charObjects.push_back(std::vector <RenderableObject*>());
		int x = 0;
		for (int j = 0; j < textRows[i].size(); j++)
		{
			Sprite* charSprite = SpriteFactory::instance()->getChar(textRows[i][j]);
			float dx = charSprite ? charSprite->rect().size.x : 7;
			_charObjects.back().push_back(new RenderableObject(this, RectF(pos.x + x, pos.y + 18.0f * i, dx, 15), charSprite, 1));
			x += dx;
		}
	}

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

	bool scroll = false;
	const Uint8* keyboard = SDL_GetKeyboardState(0);
	if (keyboard[SDL_SCANCODE_DOWN])
		scroll = true;

	if(scroll)
		for (int i = 0; i < _charObjects.size(); i++)
			for (int j = 0; j < _charObjects[i].size(); j++)
				_charObjects[i][j]->setPos(_charObjects[i][j]->pos() + PointF{0, -32* timeToSimulate });
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