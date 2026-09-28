// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "LevelLoader.h"
#include "LevelData.h"
#include "mathUtils.h"
#include "SpriteFactory.h"
#include "RenderableObject.h"
#include "PlatformerGameScene.h"
#include "OverlayScene.h"
#include "StaticObject.h"
#include "Terrain.h"
#include "Player.h"
#include "Gear.h"
#include "Box.h"
#include "Slime.h"
#include <stdexcept>

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
	PlatformerGameScene* world,
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

	for (const auto& object : level.objects())
	{
		std::string category = level.category(object);
		int layer = object.value("layer", 0);
		if (category == "Terrain")
		{
			std::vector<PointF> points = LevelData::multiline(object.at("multiline"));
			for (size_t i = 1; i < points.size(); i++)
			{
				LineF line(points[i - 1], points[i]);
				if (line.isValid())
				{
					Terrain* terrain = new Terrain(world, line);
					terrain->setLayer(layer);
				}
			}
			continue;
		}

		RotatedRectF geometry = LevelData::rotRect(object);
		Object* created = nullptr;
		if (category == "Renderable" || category == "Static")
		{
			Sprite* sprite = nullptr;
			std::string spriteName = object.value("sprite", "");
			if (!spriteName.empty())
			{
				sprite = spriteLoader->get(spriteName);
				if (!sprite)
					throw std::runtime_error("Unknown sprite: " + spriteName);
			}
			if (category == "Static")
				created = new StaticObject(world, geometry, sprite, layer);
			else
			{
				RenderableObject* rendered = new RenderableObject(world, geometry.toRect(), sprite, layer);
				rendered->setAngle(rad2deg(geometry.angle));
				created = rendered;
			}
		}
		else if (category == "Gear")
			created = new Gear(world, geometry, layer);
		else if (category == "Box")
			created = new Box(world, geometry);
		else if (category == "Slime")
			created = new Slime(world, geometry.center);
		else if (category == "Player")
		{
			if (world->player())
				throw std::runtime_error("A level must contain exactly one Player");
			created = new Player(world, geometry.center);
			world->setPlayer(created);
		}
		else
			throw std::runtime_error("Unknown Box2DPlatformer category: " + category);
		created->setLayer(layer);
	}

	if (!world->player())
		throw std::runtime_error("A level must contain exactly one Player");

	// decorative backgrounds and foregrounds, in drawing order
	for (const auto& object : level.json().value("overlays", nlohmann::ordered_json::array()))
	{
		std::string placement = object.at("placement");
		if (placement != "background" && placement != "foreground")
			throw std::runtime_error("Unknown overlay placement: " + placement);
		PointF parallax;
		if (object.contains("parallax"))
			parallax = { object.at("parallax").at("x"), object.at("parallax").at("y") };
		bool seamless = object.value("seamless", false);
		std::string spriteName = object.at("sprite");
		Sprite* sprite = spriteLoader->get(spriteName);
		if (!sprite)
			throw std::runtime_error("Unknown sprite: " + spriteName);
		OverlayScene* overlay = new OverlayScene(world, sprite, parallax, seamless);
		if (placement == "background")
			world->addBackgroundScene(overlay);
		else
			world->addForegroundScene(overlay);
	}
}

Scene* LevelLoader::load(const std::string& name)
{
	std::string path = std::string(SDL_GetBasePath()) + "levels/" + name + ".json";
	LevelData level(path);
	PlatformerGameScene* world = new PlatformerGameScene(level.sceneRect(), level.pixelUnitSize(), level.timeStep());
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
