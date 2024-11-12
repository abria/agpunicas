// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "LevelLoader.h"
#include "SpriteFactory.h"
#include "RenderableObject.h"
#include "StaticObject.h"
#include "RPGGameScene.h"
#include "Link.h"
#include "Trigger.h"
#include "Soldier.h"
#include <iostream>
#include "View.h"

using namespace agp;

LevelLoader* LevelLoader::instance()
{
	static LevelLoader uniqueInstance;
	return &uniqueInstance;
}

LevelLoader::LevelLoader()
{
	// e.g. load level maps from disk
}

Scene* LevelLoader::load(const std::string& name)
{
	SpriteFactory* spriteLoader = SpriteFactory::instance();

	if (name == "overworld")
	{
		RPGGameScene* world = new RPGGameScene(RectF(0, 0, 256, 256), { 16,16 }, 1 / 100.0f);

		// world background
		new RenderableObject(world, RectF(0, 0, 256, 256), spriteLoader->get("overworld"));
		
		// Link house (indoor)
		RenderableObject* linkHouse = new RenderableObject(world, RectF(-16, -14, 16, 14), spriteLoader->get("linkhouse"), 0);
		new StaticObject(world, RectF(-16, -9.5f, 16, 0.1f), nullptr, 1);
		new StaticObject(world, RectF(-14, -14, 0.1f, 14), nullptr, 1);
		new StaticObject(world, RectF(-2, -14, 0.1f, 14), nullptr, 1);
		new StaticObject(world, RectF(-16, -1.5f, 7, 0.1f), nullptr, 1);
		new StaticObject(world, RectF(-7, -1.5f, 7, 0.1f), nullptr, 1);

		// Outdoor static colliders
		// houses, walls, ...
		new StaticObject(world, RectF(137, 171, 6, 6), nullptr, 1);

		// NPCs
		new Soldier(world, PointF(130, 185), RectF(133, 178, 2, 3));
		
		// player
		Link* player = new Link(world, PointF(140, 179));
		world->setPlayer(player);

		// outdoor-to-indoor teleports + transitions
		new Trigger(world, RectF(139.5f, 176.5f, 1.0f, 0.5f), player, [player, world, linkHouse]()
			{
				player->setPos({-8, -7});
				world->toggleCameraFollowsPlayer();
				world->view()->setRect(linkHouse->rect());
			});

		return world;
	}
	else
	{
		std::cerr << "Unrecognized game scene name \"" << name << "\"\n";
		return nullptr;
	}
}