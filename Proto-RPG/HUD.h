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

		// raw data
		int _fps;

		// helper functions
		void moveItemSelection(int dx, int dy);

		HUD();

	public:

		virtual ~HUD() {};

		// getters/setters (to be completed)
		void setFPS(int fps);

		// actions
		void inventory(bool open);

		// extends update logic (+time management)
		virtual void update(float timeToSimulate) override;

		// extends event handler (+item selection)
		virtual void event(SDL_Event& evt) override;
};