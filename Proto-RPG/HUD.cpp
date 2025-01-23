// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "HUD.h"
#include "SpriteFactory.h"
#include "View.h"
#include "Game.h"
#include "Item.h"

using namespace agp;

HUD::HUD()
	: UIScene(RectF(0, 0, 256, 224), { 1, 1 })
{
	_coins = -1;
	_bombs = -1;
	_arrows = -1;
	_keys = -1;
	_halfHearts = -1;
	_heartsCapacity = 10;
	_fps = 0;
	_inventoryOpened = false;
	_inventoryTransition = false;
	_blocking = false;

	setBackgroundColor(Color(0, 0, 0, 0));

	// backgrounds
	new RenderableObject(this, RectF(0, 0, 256, 224), SpriteFactory::instance()->get("hud"));
	new RenderableObject(this, RectF(0, -224, 256, 224), SpriteFactory::instance()->get("inventory"));
	
	// HUD graphics
	_coinsText = new RenderableObject(this, RectI(65, 24, 3 * 7 + 2, 7), nullptr, 1);
	_bombsText = new RenderableObject(this, RectI(97, 24, 2 * 7 + 1, 7), nullptr, 1);
	_arrowsText = new RenderableObject(this, RectI(121, 24, 2 * 7 + 1, 7), nullptr, 1);
	_keysText = new RenderableObject(this, RectI(145, 24, 7, 7), nullptr, 1);
	for (int i = 0; i < 10; i++)
		_heartIcons[i] = new RenderableObject(this, RectI(161 + i * 8, 24, 7, 7), nullptr, 1);
	setCoins(0);
	setBombs(3);
	setArrows(15);
	setKeys(1);
	setHalfHearts(10);

	// inventory items
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 5; j++)
			_inventoryItems[i][j] = new Item(Item::Type(i * 5 + j));

	// inventory graphics
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 5; j++)
		{
			_inventoryIcons[i][j] = new RenderableObject(this, RectI(29 + j * 24, -244 + 50 + i * 24, 16, 16), nullptr, 2);
			_inventoryIcons[i][j]->setSprite(_inventoryItems[i][j]->icon());
		}
	_currentItemIcon = new RenderableObject(this, RectI(200, -244 + 41, 16, 16), Color(0, 0, 255), 1);
	_currentItemLabel = new RenderableObject(this, RectI(176, -244+57, 64, 16), Color(0, 0, 255), 1);
	_currentHUDItemIcon = new RenderableObject(this, RectI(40, 23, 16, 16), Color(0, 0, 255), 1);
	_selectionIcon = new RenderableObject(this, RectI(0, 0, 32, 32), SpriteFactory::instance()->get("inventory_selected"), 1);
	_currentItemIndex = { 0,0 }; 
	moveItemSelection(0, 0);

	// SNES aspect ratio
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
}

// extends update logic (+time management)
void HUD::update(float timeToSimulate)
{
	UIScene::update(timeToSimulate);

	if (!_active)
		return;

	if (_inventoryTransition && !_inventoryOpened)
		_view->move({ 0, -224 * timeToSimulate });
	else if (_inventoryTransition && _inventoryOpened)
		_view->move({ 0, 224 * timeToSimulate });

	setFPS(Game::instance()->currentFPS());
}

void HUD::inventory(bool open)
{
	// do nothing while finalizing action
	if (_inventoryTransition || open == _inventoryOpened)
		return;

	_inventoryTransition = true;

	if (open)
		_blocking = true;

	schedule("transitionOff", 1, [this, open]() {
		_inventoryTransition = false;
		_inventoryOpened = !_inventoryOpened;
		_blocking = open;
		});
}

void HUD::setCoins(int newCoins)
{
	if (newCoins == _coins)
		return;

	_coins = newCoins;
	_coinsText->setSprite(SpriteFactory::instance()->getNumberHUD(_coins, 3));
}

void HUD::setBombs(int newBombs)
{
	if (newBombs == _bombs)
		return;

	_bombs = newBombs;
	_bombsText->setSprite(SpriteFactory::instance()->getNumberHUD(_bombs, 2));
}

void HUD::setArrows(int newArrows)
{
	if (newArrows == _arrows)
		return;

	_arrows = newArrows;
	_arrowsText->setSprite(SpriteFactory::instance()->getNumberHUD(_arrows, 2));
}

void HUD::setKeys(int newKeys)
{
	if (newKeys == _keys)
		return;

	_keys = newKeys;
	_keysText->setSprite(SpriteFactory::instance()->getNumberHUD(_keys, 1));
}

void HUD::refreshHearts()
{
	int fullHearts = _halfHearts / 2;
	int halfHearts = _halfHearts % 2;
	for (int i = 0; i < 10; i++)
	{
		if (i < fullHearts)
			_heartIcons[i]->setSprite(SpriteFactory::instance()->get("hud_heart"), true);
		else if (halfHearts && i == fullHearts)
			_heartIcons[i]->setSprite(SpriteFactory::instance()->get("hud_heart_half"), true);
		else if (i < _heartsCapacity)
			_heartIcons[i]->setSprite(SpriteFactory::instance()->get("hud_heart_empty"), true);
		else
			_heartIcons[i]->setSprite(nullptr);
	}
}

void HUD::setHalfHearts(int newHalfHearts)
{
	if (newHalfHearts == _halfHearts)
		return;

	_halfHearts = newHalfHearts;
	refreshHearts();
}

void HUD::setHeartsCapacity(int newCapacity)
{
	if (_heartsCapacity == newCapacity)
		return;

	_heartsCapacity = newCapacity;
	refreshHearts();
}

void HUD::setFPS(int fps) 
{ 
	setCoins(fps);
	/*if (fps != _fps)
	{
		_fps = fps;
		_fpsObj->setSprite(SpriteFactory::instance()->getText(std::to_string(_fps), { 0.5f, 0.5f }), true);
	}*/
}

void HUD::moveItemSelection(int dx, int dy)
{
	_currentItemIndex += Point(dx, dy);
	_currentItemIndex.x = std::min(_currentItemIndex.x, 4);
	_currentItemIndex.x = std::max(_currentItemIndex.x, 0);
	_currentItemIndex.y = std::min(_currentItemIndex.y, 3);
	_currentItemIndex.y = std::max(_currentItemIndex.y, 0);
	_selectionIcon->setPos(_inventoryIcons[_currentItemIndex.y][_currentItemIndex.x]->rect().pos - PointF{ 8, 8 });
	_currentItemIcon->setSprite(_inventoryItems[_currentItemIndex.y][_currentItemIndex.x]->icon());
	_currentItemLabel->setSprite(_inventoryItems[_currentItemIndex.y][_currentItemIndex.x]->label());
	_currentHUDItemIcon->setSprite(_inventoryItems[_currentItemIndex.y][_currentItemIndex.x]->icon());
}

void HUD::event(SDL_Event& evt)
{
	UIScene::event(evt);

	if (_inventoryTransition || !_inventoryOpened)
		return;

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.scancode == SDL_SCANCODE_RETURN)
			inventory(false);
		else if (evt.key.keysym.scancode == SDL_SCANCODE_RIGHT)
			moveItemSelection(1, 0);
		else if (evt.key.keysym.scancode == SDL_SCANCODE_LEFT)
			moveItemSelection(-1, 0);
		else if (evt.key.keysym.scancode == SDL_SCANCODE_UP)
			moveItemSelection(0, -1);
		else if (evt.key.keysym.scancode == SDL_SCANCODE_DOWN)
			moveItemSelection(0, 1);
	}
}
