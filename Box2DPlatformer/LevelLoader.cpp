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
#include "ComplexPlatformerGameScene.h"
#include "OverlayScene.h"
#include "StaticObject.h"
#include "Terrain.h"
#include "Player.h"
#include "Gear.h"
#include "Box.h"
#include "Slime.h"
#include <memory>
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

Scene* LevelLoader::load(const std::string& name)
{
	std::string path = std::string(SDL_GetBasePath()) + "levels/" + name + ".json";
	LevelData level(path);
	auto world = std::make_unique<ComplexPlatformerGameScene>(level.sceneRect(), level.pixelUnitSize(), level.timeStep());
	level.configure(world.get());

	auto sprite = [](const std::string& id) -> Sprite*
	{
		if (id.empty())
			return nullptr;
		Sprite* result = SpriteFactory::instance()->get(id);
		if (!result)
			throw std::runtime_error("Unknown sprite: " + id);
		return result;
	};

	// world images are not editable regions
	for (const auto& object : level.json().value("backgroundImages", nlohmann::ordered_json::array()))
	{
		RotatedRectF geometry = LevelData::rotRect(object);
		auto image = new RenderableObject(world.get(), geometry.toRect(), sprite(object.at("sprite")), object.value("layer", -1));
		image->setAngle(rad2deg(geometry.angle));
		world->addBackgroundImage(image);
	}

	for (const auto& object : level.objects())
	{
		std::string category = level.category(object);
		int layer = object.value("layer", 0);
		if (category == "Terrain")
		{
			auto points = LevelData::multiline(object.at("multiline"));
			for (size_t i = 1; i < points.size(); i++)
			{
				LineF line(points[i - 1], points[i]);
				if (line.isValid())
					new Terrain(world.get(), line, sprite(object.value("sprite", "")), layer);
			}
			continue;
		}

		RotatedRectF geometry = LevelData::rotRect(object);
		Object* created = nullptr;
		if (category == "Renderable")
		{
			auto rendered = new RenderableObject(world.get(), geometry.toRect(), sprite(object.value("sprite", "")), layer);
			rendered->setAngle(rad2deg(geometry.angle));
			created = rendered;
		}
		else if (category == "Static")
			created = new StaticObject(world.get(), geometry, sprite(object.value("sprite", "")), layer);
		else if (category == "Gear")
			created = new Gear(world.get(), geometry, sprite(object.value("sprite", "gear")), layer);
		else if (category == "Box")
			created = new Box(world.get(), geometry);
		else if (category == "Slime")
			created = new Slime(world.get(), geometry.center);
		else if (category == "Player")
		{
			if (world->player())
				throw std::runtime_error("A level must contain exactly one Player");
			created = new Player(world.get(), geometry.center);
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
		auto overlay = new OverlayScene(world.get(), sprite(object.at("sprite")), parallax, seamless);
		if (placement == "background")
			world->addBackgroundScene(overlay);
		else
			world->addForegroundScene(overlay);
	}

	return world.release();
}
