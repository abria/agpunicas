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
#include "Singleton.h"

namespace agp
{
	class HUD;
	class Item;
}

// HUD
// - implements (as an example) the Super Mario Bros's HUD
class agp::HUD : public UIScene, public Singleton<HUD>
{
	friend class Singleton<HUD>;

	protected:

		// hud
		int _coins;
		int _bombs;
		int _arrows;
		int _keys;
		int _halfHearts;
		int _heartsCapacity;
		RenderableObject* _coinsText;
		RenderableObject* _bombsText;
		RenderableObject* _arrowsText;
		RenderableObject* _keysText;
		RenderableObject* _heartIcons[10];

		// inventory
		bool _inventoryOpened;
		bool _inventoryTransition;
		Item* _inventoryItems[4][5];
		RenderableObject* _inventoryIcons[4][5];
		Point _currentItemIndex;
		RenderableObject* _currentItemIcon;
		RenderableObject* _currentItemLabel;
		RenderableObject* _currentHUDItemIcon;
		RenderableObject* _selectionIcon;

		// others
		int _fps;

		// helper functions
		void moveItemSelection(int dx, int dy);
		void refreshHearts();

		HUD();

	public:

		virtual ~HUD() {};

		// getters/setters
		void setCoins(int newCoins);
		void setBombs(int newBombs);
		void setArrows(int newArrows);
		void setKeys(int newKeys);
		void setHalfHearts(int newHalfHearts);
		void setHeartsCapacity(int newCapacity);
		void setFPS(int fps);

		// actions
		void inventory(bool open);

		// extends update logic (+time management)
		virtual void update(float timeToSimulate) override;

		// extends event handler (+item selection)
		virtual void event(SDL_Event& evt) override;
};