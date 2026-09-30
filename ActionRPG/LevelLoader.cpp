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
#include "Soldier.h"
#include <iostream>
#include <stdexcept>
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
	const LevelData& level)
{
	SpriteFactory* spriteLoader = SpriteFactory::instance();
	level.configure(world);

	// world images are not editable regions
	for (const auto& object : level.json().value("backgroundImages", nlohmann::ordered_json::array()))
	{
		RotatedRectF geometry = LevelData::rotRect(object);
		std::string spriteName = object.at("sprite");
		Sprite* sprite = spriteLoader->get(spriteName);
		if (!sprite)
			throw std::runtime_error("Unknown sprite: " + spriteName);
		RenderableObject* image = new RenderableObject(world, geometry.toRect(), sprite, object.value("layer", -1));
		image->setAngle(rad2deg(geometry.angle));
		world->addBackgroundImage(image);
	}

	// portals need the player, regardless of json object order
	for (const auto& object : level.objects())
		if (level.category(object) == "Link")
		{
			if (world->player())
				throw std::runtime_error("A level must contain exactly one Link");
			world->setPlayer(new Link(world, LevelData::rotRect(object).toRect().pos));
		}
	if (!world->player())
		throw std::runtime_error("A level must contain exactly one Link");

	// portals with matching names = portals to be connected
	std::map<std::string, std::vector<Portal*>> portals;

	for (const auto& jObj : level.objects())
	{
		std::string category = level.category(jObj);
		if (category == "Link")
			continue;
		
		if (jObj.contains("rect") || jObj.contains("rotRect"))
		{
			RotatedRectF rrect = LevelData::rotRect(jObj);

			if (category == "NPC")
				new NPC(world, rrect.toRect().pos);
			else if (category == "Soldier")
				new Soldier(world, rrect.toRect().pos, LevelData::rect(jObj.at("patrolRect")));
			else if (category == "Static" || category == "Bush")
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
	std::string path = std::string(SDL_GetBasePath()) + "levels/" + name + ".json";
	LevelData level(path);
	RPGGameScene* world = new RPGGameScene(level.sceneRect(), level.pixelUnitSize(), level.timeStep());
	try
	{
		loadJson(world, level);
	}
	catch (...)
	{
		delete world;
		throw;
	}
	return world;
}
