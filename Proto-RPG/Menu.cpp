// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Menu.h"
#include "SpriteFactory.h"
#include "View.h"
#include "Game.h"
#include "Window.h"
#include "Audio.h"

using namespace agp;

MenuItem::MenuItem(Menu* container, int index, const std::string& text, std::function<void()> task)
	: RenderableObject(container, 
		RectF(
			container->menuRect().left(), 
			container->menuRect().top() + 0.5f + 0.7f * index, 
			container->menuRect().size.x, 0.5f),
		SpriteFactory::instance()->getTextSMB(' ' + text, { 0.5f, 0.5f }, 0, ' ', false), 1)
{
	_container = container;
	_index = index;
	_text = text;
	_task = task;
	_selected = false;
}

void MenuItem::refresh()
{
	setSprite(SpriteFactory::instance()->getTextSMB(' ' + _text, { 0.5f + _selected/30.0f, 0.5f + _selected / 30.0f }, 0, ' ', _selected), true);
}

void MenuItem::update(float dt)
{
	RenderableObject::update(dt);

	_focused = _selected;
}

Menu::Menu(const PointF& position, float width, Menu* parent, bool closable)
	: UIScene(RectF(0, 0, 16, 14), { 16,16 })
{
	_parent = parent;
	_itemSelected = 0;
	_menuRect = { position.x, position.y, width, 0.5f };
	_closable = closable;

	// menu layer
	_backgroundColor = { 0, 0, 0, 255 };
	_menuBackground = new RenderableObject(this, _menuRect, Color(200, 76, 12, 255));

	// default: modal menu (blocks all lower scenes)
	_blocking = true;

	// SNES aspect ratio
	_view->setFixedAspectRatio(Game::instance()->aspectRatio());
}

Menu::Menu(Menu* parent)
	: Menu(parent->menuRect().tl(), parent->menuRect().size.x, parent)
{
}

MenuItem* Menu::addItem(const std::string& text, std::function<void()> task)
{
	MenuItem* item = new MenuItem(this, int(_items.size()), text, task);
	_items.push_back(item);
	if (_items.size() == 1)
		item->setSelected(true);

	_menuRect.size.y = 0.5f + _items.size() * 0.7f + 0.3f;
	_menuBackground->setRect(_menuRect);

	return item;
}

void Menu::event(SDL_Event& evt)
{
	UIScene::event(evt);

	if (evt.type == SDL_KEYDOWN)
	{
		if (evt.key.keysym.scancode == SDL_SCANCODE_DOWN)
		{
			Audio::instance()->playSound("fireball");
			_items[_itemSelected]->setSelected(false);
			_itemSelected = (_itemSelected + 1) % int(_items.size());
			_items[_itemSelected]->setSelected(true);
		}
		else if (evt.key.keysym.scancode == SDL_SCANCODE_UP)
		{
			Audio::instance()->playSound("fireball");
			_items[_itemSelected]->setSelected(false);
			_itemSelected = (_itemSelected - 1) < 0 ? int(_items.size()) - 1 : _itemSelected - 1;
			_items[_itemSelected]->setSelected(true);
		}
		else if (evt.key.keysym.scancode == SDL_SCANCODE_RETURN || evt.key.keysym.scancode == SDL_SCANCODE_RIGHT)
		{
			Audio::instance()->playSound("kick");
			_items[_itemSelected]->run();
		}
		else if (_closable && (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE || evt.key.keysym.scancode == SDL_SCANCODE_LEFT))
		{
			Audio::instance()->playSound("smash");
			Game::instance()->popSceneLater();
		}
	}
}

Menu* Menu::mainMenu()
{
	Menu* menu = new Menu({ 3.0, 10.5f }, 10.0, 0, false);

	new RenderableObject(menu, RectF(0, -1, 16, 13), SpriteFactory::instance()->get("welcome"), -1);

	menu->addItem("New Game", []() 
	{
		Game::instance()->popSceneLater();
		//Audio::instance()->playMusic("overworld");
	});
	menu->addItem("Options", [menu]()
	{
		Menu* nestedMenu = new Menu(menu);
		nestedMenu->addItem("Commands", []() {printf("Commands pressed\n"); });
		nestedMenu->addItem("Volume", []() {printf("Volume pressed\n"); });

		SDL_RendererInfo info;
		SDL_GetRendererInfo(Game::instance()->window()->renderer(), &info);
		bool vsyncOn = info.flags & SDL_RENDERER_PRESENTVSYNC;
		nestedMenu->addItem(std::string("VSYNC ") + (vsyncOn ? "on" : "off"), [nestedMenu]()
			{
				bool vsyncOn = nestedMenu->itemAt(2)->text() == "VSYNC on";
				nestedMenu->itemAt(2)->setText(vsyncOn ? "VSYNC off" : "VSYNC on");
				SDL_RenderSetVSync(Game::instance()->window()->renderer(), !vsyncOn);
			});
		Game::instance()->pushScene(nestedMenu);
	});
	menu->addItem("Quit", []() {Game::instance()->quit(); });

	return menu;
}

Menu* Menu::pauseMenu()
{
	Menu* menu = new Menu({ 1.5, 3 }, 5, 0, false);
	menu->addItem("Resume", []() 
		{
			Game::instance()->popSceneLater(); 
			Audio::instance()->resumeMusic();
		});
	menu->addItem("Reset", []()
		{
			Game::instance()->reset();
		});
	menu->addItem("Quit", []() {Game::instance()->quit(); });

	Audio::instance()->playSound("pause");
	Audio::instance()->pauseMusic();

	return menu;
}