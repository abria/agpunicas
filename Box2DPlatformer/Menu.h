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
#include <string>
#include <vector>
#include <functional>

namespace agp
{
	class MenuItem;
	class Menu;
}

// MenuItem
// - implements a simple textSprite-based menu item
class agp::MenuItem : public RenderableObject
{
	protected:

		Menu* _container;
		int _index;
		int _selected;
		std::string _text;
		std::function<void()> _task;	// performed when Enter pressed on this item

	public:

		MenuItem(Menu* container, int index, const std::string& text, std::function<void()> task);
		virtual ~MenuItem() {};	

		bool selected() { return _selected; }
		void setSelected(bool on) { _selected = on; refresh(); }
		const std::string& text() { return _text; }
		void setText(const std::string& text) { _text = text; refresh();}
		void refresh();

		void run() { _task(); }

		// extends update logic (+selection)
		virtual void update(float dt) override;
};

// Menu
// - implements a simple multiple-choice Menu
class agp::Menu : public UIScene
{
	protected:

		Menu* _parent;
		RectF _menuRect;
		int _itemSelected;
		bool _closable;
		std::vector< MenuItem* > _items;
		RenderableObject* _transparentLayer;
		RenderableObject* _menuBackground;

	public:

		Menu(const PointF& position, float width, Menu* parent = 0, bool closable = true);
		Menu(Menu* parent, float width);
		virtual ~Menu() {};

		// getters/setters (to be completed)
		const RectF& menuRect() { return _menuRect; }
		MenuItem* addItem(const std::string& text, std::function<void()> task);
		MenuItem* itemAt(int index) { return _items[index]; }

		// extends event handler (+menu selection)
		virtual void event(SDL_Event& evt) override;

		// provide main menus
		static Menu* mainMenu();
		static Menu* pauseMenu();
};