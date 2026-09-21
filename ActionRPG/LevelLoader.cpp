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
#include <fstream>
#include "View.h"
#include "json.hpp"
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
	const std::string& jsonPath,
	Link* link)
{
	std::ifstream f(jsonPath);
	if (!f.is_open())
		return;

	nlohmann::json jRoot = nlohmann::json::parse(f);
	std::vector <std::string> _categories = jRoot["categories"].get<std::vector<std::string>>();
	std::vector<nlohmann::json> jsonObjects = jRoot["objects"].get<std::vector<nlohmann::json>>();
	
	// portals with matching names = portals to be connected
	std::map<std::string, std::vector<Portal*>> portals;

	for (auto& jObj : jsonObjects)
	{
		int category = jObj["category"];
		std::string name = jObj["name"];
		
		if (jObj.contains("rect") || jObj.contains("rotRect"))
		{
			RotatedRectF rrect;

			if (jObj.contains("rect"))
			{
				RectF rect;
				rect.pos.x = jObj["rect"]["x"];
				rect.pos.y = jObj["rect"]["y"];
				rect.size.x = jObj["rect"]["width"];
				rect.size.y = jObj["rect"]["height"];
				rect.yUp = jObj["rect"]["yUp"];
				rrect = rect;
			}
			else
			{
				rrect.center.x = jObj["rotRect"]["cx"];
				rrect.center.y = jObj["rotRect"]["cy"];
				rrect.size.x = jObj["rotRect"]["width"];
				rrect.size.y = jObj["rotRect"]["height"];
				rrect.angle = deg2rad(float(jObj["rotRect"]["angle"]));
				rrect.yUp = jObj["rotRect"]["yUp"];
			}

			if (_categories[category] == "Static" || _categories[category] == "Bush")
				new StaticObject(world, rrect, nullptr, 1);
			else if (_categories[category] == "Portal")
				portals[name].push_back(new Portal(world, rrect));
			else if (_categories[category] == "Clipper")
				new Clipper(world, rrect.toRect());
		}
		else if (jObj.contains("multiline"))
		{
			std::vector<nlohmann::json> jsonPoints = jObj["multiline"].get<std::vector<nlohmann::json>>();
			for (int i = 0; i < jsonPoints.size() - 1; i++)
			{
				float x1 = jsonPoints[i]["x"];
				float y1 = jsonPoints[i]["y"];
				float x2 = jsonPoints[i + 1]["x"];
				float y2 = jsonPoints[i + 1]["y"];
				LineF line(x1, y1, x2, y2);
				if(line.isValid())
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

	f.close();
}

Scene* LevelLoader::load(const std::string& name)
{
	SpriteFactory* spriteLoader = SpriteFactory::instance();

	if (name == "overworld")
	{
		RPGGameScene* world = new RPGGameScene(RectF(0, 0, 256, 256), { 16,16 }, 1 / 100.0f);
		world->setBackgroundColor({ 128, 128, 128 });

		// backgrounds
		world->addBackgroundImage(new RenderableObject(world, RectF(0, 0, 256, 256), spriteLoader->get("overworld")));
		world->addBackgroundImage(new RenderableObject(world, RectF(-16, -14, 16, 14), spriteLoader->get("linkhouse"), 0));

		// NPCs
		new NPC(world, PointF(-8, -7));
		new Soldier(world, PointF(130, 185), RectF(133, 178, 2, 3));
		
		// player
		Link* player = new Link(world, PointF(140, 179));
		//Link* player = new Link(world, PointF(138, 189));
		world->setPlayer(player);

		//new StaticObject(world, RectF(137, 171, 2.5, 6), nullptr, 5);

		//new StaticObject(world, RotatedRectF(140, 185, 5, 2, PI/8), spriteLoader->get("linkhouse"), 2);

		// load jObj and convert regions to game objects
		loadJson(world, std::string(SDL_GetBasePath()) + "EditorScene.json", player);

		return world;
	}
	else
	{
		std::cerr << "Unrecognized game scene name \"" << name << "\"\n";
		return nullptr;
	}
}