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

void LevelLoader::loadJson(
	PlatformerGameScene* world,
	const LevelData& level)
{
	SpriteFactory* spriteLoader = SpriteFactory::instance();

	// Ground graphics; the Terrain lines in the editor define its collisions.
	world->addBackgroundImage(new RenderableObject(world,
		RectF(0, 0, 96, 13, true), spriteLoader->get("fg_ground"), -1));

	for (const auto& object : level.objects())
	{
		std::string category = level.category(object);
		if (category == "Terrain")
		{
			std::vector<PointF> points = LevelData::multiline(object.at("multiline"));
			for (size_t i = 1; i < points.size(); i++)
			{
				LineF line(points[i - 1], points[i]);
				if (line.isValid())
					new Terrain(world, line);
			}
			continue;
		}

		RotatedRectF geometry = LevelData::rotRect(object);
		if (category == "Static")
			new StaticObject(world, geometry);
		else if (category == "Renderable")
		{
			auto* rendered = new RenderableObject(world, geometry.toRect(), nullptr);
			rendered->setAngle(rad2deg(geometry.angle));
		}
		else if (category == "Gear")
			new Gear(world, geometry);
		else if (category == "Box")
			new Box(world, geometry);
		else if (category == "Slime")
			new Slime(world, geometry.center);
		else if (category == "Player")
		{
			if (world->player())
				throw std::runtime_error("A level must contain exactly one Player");
			world->setPlayer(new Player(world, geometry.center));
		}
		else
			throw std::runtime_error("Unknown Box2DPlatformer category: " + category);
	}

	if (!world->player())
		throw std::runtime_error("A level must contain exactly one Player");

	// Backgrounds, from farthest to nearest. Parallax is relative to the camera.
	world->addBackgroundScene(new OverlayScene(world, spriteLoader->get("bg_sky")));
	world->addBackgroundScene(new OverlayScene(world, spriteLoader->get("bg_houses"), {0.2f, 1}, true));
	world->addBackgroundScene(new OverlayScene(world, spriteLoader->get("bg_grass"), {0.4f, 1}, true));

	// Foreground effects, drawn over the game.
	world->addForegroundScene(new OverlayScene(world, spriteLoader->get("rain")));
	world->addForegroundScene(new OverlayScene(world, spriteLoader->get("fg_fog"), {1.2f, 1}, true));
}

Scene* LevelLoader::load(const std::string& name)
{
	std::string path = std::string(SDL_GetBasePath()) + "levels/" + name + ".json";
	LevelData level(path);
	PlatformerGameScene* world = new PlatformerGameScene(RectF(0, 0, 96, 13, true), {32, 32}, 0.01f, {0, 0, 0, 0});
	try
	{
		world->setJsonPath(path);
		loadJson(world, level);
	}
	catch (...)
	{
		delete world;
		throw;
	}
	return world;
}
