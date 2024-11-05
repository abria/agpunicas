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
#include "ComplexPlatformerGameScene.h"
#include "OverlayScene.h"
#include "StaticObject.h"
#include "Terrain.h"
#include "Player.h"
#include "mathUtils.h"
#include "Gear.h"
#include "Box.h"
#include "Slime.h"
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
	
	ComplexPlatformerGameScene* world = nullptr;

	if (name == "level0")
	{
		world = new ComplexPlatformerGameScene(RectF(0, 0, 96, 13, true), {32, 32}, 1 / 100.0f);

		// game foreground
		new RenderableObject(world, world->rect(), spriteLoader->get("fg_ground"), 0);

		// terrain
		Terrain::TerrainSequence(world, { 0, 3.3f },
			{
				{9, 0}, {4, 2}, {2, 0}, {4, -2}, {13, 0}, {4, -2},
				{2, 0}, {10, 5}, {14.5f, 0}, {4, -2}, {3, 0}, {2, 1},
				{1.5f, 0}, {1.5f, 1}, {2, 0}, {4, -2}, {15.5f, 0}, {20, 0}
			});

		// dynamic objects
		//new Box(world, RotatedRectF({ 5, 10 }, { 1,1 }, 0, true));
		//new Slime(world, { 10, 15 });

		// kinematic objects
		new Gear(world, RotatedRectF({ 90, 2 }, { 15, 15 }, 0, true), spriteLoader->get("gear"), -1);
		new Gear(world, RotatedRectF({ 103, 2 }, { 15, 15 }, 0, true), spriteLoader->get("gear"), 1);

		// player
		world->setPlayer(new Player(world, { 3, 10 }));
		
		// decorative backgrounds and foregrounds
		world->addBackgroundScene(new OverlayScene(world, spriteLoader->get("bg_sky")));
		world->addBackgroundScene(new OverlayScene(world, spriteLoader->get("bg_houses"), { 0.2f, 1 }, true));
		world->addBackgroundScene(new OverlayScene(world, spriteLoader->get("bg_grass"), { 0.4f, 1 }, true));
		world->addForegroundScene(new OverlayScene(world, spriteLoader->get("rain")));
		world->addForegroundScene(new OverlayScene(world, spriteLoader->get("fg_fog"), { 1.2f, 1 }, true));
	}
	else
		std::cerr << "Unrecognized game scene name \"" << name << "\"\n";

	return world;
}