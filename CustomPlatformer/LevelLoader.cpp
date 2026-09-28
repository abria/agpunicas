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
#include "SpriteFactory.h"
#include "StaticObject.h"
#include "PlatformerGameScene.h"
#include "Mario.h"
#include "HammerBrother.h"
#include "Lift.h"
#include "Trigger.h"
#include <stdexcept>

using namespace agp;

LevelLoader::LevelLoader()
{
	// e.g. load level data from disk
}

void LevelLoader::loadJson(
	PlatformerGameScene* world,
	const LevelData& level)
{
	SpriteFactory* spriteLoader = SpriteFactory::instance();
	level.configure(world);
	std::vector<Object*> lifts;

	for (const auto& object : level.objects())
	{
		std::string category = level.category(object);
		RotatedRectF geometry = LevelData::rotRect(object);
		if (geometry.angle != 0)
			throw std::runtime_error("CustomPlatformer requires axis-aligned rectangles");
		RectF rect = geometry.toRect();
		int layer = object.value("layer", 0);
		Object* created = nullptr;

		if (category == "Static")
		{
			Sprite* sprite = nullptr;
			std::string spriteName = object.value("sprite", "");
			if (!spriteName.empty())
			{
				sprite = spriteLoader->get(spriteName);
				if (!sprite)
					throw std::runtime_error("Unknown sprite: " + spriteName);
			}
			created = new StaticObject(world, rect, sprite, layer);
		}
		else if (category == "HammerBrother")
			// constructor uses a sprite-specific spawn offset
			created = new HammerBrother(world, rect.pos + PointF(-1 / 16.0f, 1));
		else if (category == "Lift")
		{
			float range = object.value("range", 3.0f);
			if (!std::isfinite(range) || range <= 0)
				throw std::runtime_error("Lift range must be positive");
			created = new Lift(world, rect, object.value("vertical", true), range, layer);
			lifts.push_back(created);
		}
		else if (category == "Mario")
		{
			if (world->player())
				throw std::runtime_error("A level must contain exactly one Mario");
			created = new Mario(world, rect.pos - PointF(1 / 16.0f, 0));
			world->setPlayer(created);
		}
		else
			throw std::runtime_error("Unknown CustomPlatformer category: " + category);

		created->setLayer(layer);
	}

	if (!world->player())
		throw std::runtime_error("A level must contain exactly one Mario");

	// trigger example
	if (!lifts.empty())
		new Trigger(world, RectF(1, -12, 0.5f, 13), world->player()->to<CollidableObject*>(), [lifts]()
			{
				for (auto lift : lifts)
					lift->toggleFreezed();
			});
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
