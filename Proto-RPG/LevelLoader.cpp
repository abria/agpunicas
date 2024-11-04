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

		new RenderableObject(world, RectF(0, 0, 256, 256), spriteLoader->get("overworld"));
		
		new Soldier(world, PointF(135, 185), RectF(140, 179, 4, 7));

		// player
		Link* player = new Link(world, PointF(140, 179));
		world->setPlayer(player);

		return world;
	}
	else
	{
		std::cerr << "Unrecognized game scene name \"" << name << "\"\n";
		return nullptr;
	}
}