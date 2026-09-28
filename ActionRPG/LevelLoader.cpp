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
#include <memory>
#include <stdexcept>
#include "View.h"
#include "LevelData.h"
#include "Portal.h"
#include "mathUtils.h"
#include "NPC.h"
#include "Clipper.h"

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

void LevelLoader::loadJson(
	RPGGameScene* world, 
	const std::string& jsonPath)
{
	LevelData level(jsonPath);
	level.configure(world);

	// portals with matching names = portals to be connected
	std::map<std::string, std::vector<Portal*>> portals;

	for (const auto& jObj : level.objects())
	{
		std::string category = level.category(jObj);
		
		if (jObj.contains("rect") || jObj.contains("rotRect"))
		{
			RotatedRectF rrect = LevelData::rotRect(jObj);

			if (category == "Static" || category == "Bush")
				new StaticObject(world, rrect, nullptr, 1);
			else if (category == "Portal")
			{
				std::string name = jObj.value("name", "");
				if (name.empty())
					throw std::runtime_error("A Portal needs a name to identify its destination");
				portals[name].push_back(new Portal(world, rrect));
			}
			else if (category == "Clipper")
				new Clipper(world, rrect.toRect());
		}
		else if (jObj.contains("multiline"))
		{
			auto points = LevelData::multiline(jObj.at("multiline"));
			for (size_t i = 1; i < points.size(); i++)
			{
				LineF line(points[i - 1], points[i]);
				if (line.isValid())
					new StaticObject(world, RotatedRectF(line, 0.1f, false), nullptr, 2);
			}
		}
	}

	// connect paired portals
	for (auto& pair : portals)
		if (pair.second.size() == 2)
		{
			pair.second[0]->setDestination(pair.second[1]);
			pair.second[1]->setDestination(pair.second[0]);
		}
		else
			std::cerr << "Found " << pair.second.size() << " portals with name " << pair.first << ": expected 2\n";

}

Scene* LevelLoader::load(const std::string& name)
{
	SpriteFactory* spriteLoader = SpriteFactory::instance();

	if (name == "overworld")
	{
		auto world = std::make_unique<RPGGameScene>(RectF(0, 0, 256, 256), Point(16, 16), 1 / 100.0f);
		world->setBackgroundColor({ 128, 128, 128 });

		// backgrounds
		world->addBackgroundImage(new RenderableObject(world.get(), RectF(0, 0, 256, 256), spriteLoader->get("overworld")));
		world->addBackgroundImage(new RenderableObject(world.get(), RectF(-16, -14, 16, 14), spriteLoader->get("linkhouse"), 0));

		// NPCs
		new NPC(world.get(), PointF(-8, -7));
		new Soldier(world.get(), PointF(130, 185), RectF(133, 178, 2, 3));
		
		// player
		Link* player = new Link(world.get(), PointF(140, 179));
		//Link* player = new Link(world.get(), PointF(138, 189));
		world->setPlayer(player);

		//new StaticObject(world.get(), RectF(137, 171, 2.5, 6), nullptr, 5);

		//new StaticObject(world.get(), RotatedRectF(140, 185, 5, 2, PI/8), spriteLoader->get("linkhouse"), 2);

		// load jObj and convert regions to game objects
		loadJson(world.get(), std::string(SDL_GetBasePath()) + "EditorScene.json");

		return world.release();
	}
	else
	{
		std::cerr << "Unrecognized game scene name \"" << name << "\"\n";
		return nullptr;
	}
}