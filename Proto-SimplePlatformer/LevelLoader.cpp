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
#include "GameScene.h"
#include "Mario.h"
#include "HammerBrother.h"
#include "Lift.h"
#include "Trigger.h"
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

GameScene* LevelLoader::load(const std::string& name)
{
	SpriteFactory* spriteLoader = SpriteFactory::instance();

	if (name == "supermario")
	{
		GameScene* world = new GameScene(RectF(0, -12, 224, 15), 1 / 100.0f);
		world->setBackgroundColor(Color(92, 148, 252));

		// terrain
		new StaticObject(world, RectF(0, 1, 68, 2),  spriteLoader->get("terrain"));
		// boxes
		new StaticObject(world, RectF(16, -3, 1, 1), spriteLoader->get("box"));
		new StaticObject(world, RectF(21, -3, 1, 1), spriteLoader->get("box"));
		new StaticObject(world, RectF(22, -7, 1, 1), spriteLoader->get("box"));
		new StaticObject(world, RectF(23, -3, 1, 1), spriteLoader->get("box"));
		// bricks
		new StaticObject(world, RectF(20, -3, 1, 1), spriteLoader->get("brick"));
		new StaticObject(world, RectF(22, -3, 1, 1), spriteLoader->get("brick"));
		new StaticObject(world, RectF(24, -3, 1, 1), spriteLoader->get("brick"));
		// pipes
		new StaticObject(world, RectF(28, -1, 2, 4), spriteLoader->get("pipe-small"), -1);
		new StaticObject(world, RectF(38, -2, 2, 5), spriteLoader->get("pipe-med"), -1);
		new StaticObject(world, RectF(46, -3, 2, 6), spriteLoader->get("pipe-big"), -1);
		new StaticObject(world, RectF(57, -3, 2, 6), spriteLoader->get("pipe-big"), -1);

		//new RenderableObject(world, RectF(0, -5, 30, 20), Color(0, 0, 0, 255), 2);

		// hammer brother debug
		/*for (int i = 0; i < 8; i++)
		{
			new StaticObject(world, RectF(7 + i, -3, 1, 1), spriteLoader->get("brick"));
			new StaticObject(world, RectF(7 + i, -7, 1, 1), spriteLoader->get("brick"));
		}*/
		new HammerBrother(world, PointF(21, 0));
		new HammerBrother(world, PointF(23, -4));

		// lifts
		Lift* lift1 = new Lift(world, RectF(7, -2, 3, 0.5f), spriteLoader->get("platform"), false, 10, 10);
		Lift* lift2 = new Lift(world, RectF(4, -4, 3, 0.5f), spriteLoader->get("platform"), true, 3, 10);

		// mario
		Mario* mario = new Mario(world, PointF(2.5, 0));
		world->setPlayer(mario);

		// trigger example
		new Trigger(world, RectF(1, -12, 0.5, 13), mario, [lift1, lift2]()
			{
				lift1->toggleFreezed();
				lift2->toggleFreezed();
			});

		return world;
	}
	else
	{
		std::cerr << "Unrecognized game scene name \"" << name << "\"\n";
		return nullptr;
	}
}