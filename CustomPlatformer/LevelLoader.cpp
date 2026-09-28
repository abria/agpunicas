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
#include <map>
#include <memory>
#include <stdexcept>

using namespace agp;

LevelLoader::LevelLoader()
{
	// e.g. load level data from disk
}

Scene* LevelLoader::load(const std::string& name)
{
	std::string path = std::string(SDL_GetBasePath()) + "levels/" + name + ".json";
	LevelData level(path);
	auto world = std::make_unique<PlatformerGameScene>(level.sceneRect(), level.pixelUnitSize(), level.timeStep());
	level.configure(world.get());
	std::map<std::string, Object*> objectsById;

	auto sprite = [](const std::string& id) -> Sprite*
	{
		if (id.empty())
			return nullptr;
		Sprite* result = SpriteFactory::instance()->get(id);
		if (!result)
			throw std::runtime_error("Unknown sprite: " + id);
		return result;
	};

	for (const auto& object : level.objects())
	{
		std::string category = level.category(object);
		if (category == "Trigger")
			continue;

		RotatedRectF geometry = LevelData::rotRect(object);
		if (geometry.angle != 0)
			throw std::runtime_error("CustomPlatformer requires axis-aligned rectangles");
		RectF rect = geometry.toRect();
		int layer = object.value("layer", 0);
		Object* created = nullptr;

		if (category == "Static")
			created = new StaticObject(world.get(), rect, sprite(object.value("sprite", "")), layer);
		else if (category == "HammerBrother")
			// constructor uses a sprite-specific spawn offset
			created = new HammerBrother(world.get(), rect.pos + PointF(-1 / 16.0f, 1));
		else if (category == "Lift")
		{
			float range = object.value("range", 3.0f);
			if (!std::isfinite(range) || range <= 0)
				throw std::runtime_error("Lift range must be positive");
			created = new Lift(world.get(), rect, sprite(object.value("sprite", "platform")),
				object.value("vertical", true), range, layer);
		}
		else if (category == "Mario")
		{
			if (world->player())
				throw std::runtime_error("A level must contain exactly one Mario");
			created = new Mario(world.get(), rect.pos - PointF(1 / 16.0f, 0));
			world->setPlayer(created);
		}
		else
			throw std::runtime_error("Unknown CustomPlatformer category: " + category);

		created->setLayer(layer);
		std::string id = object.value("id", "");
		if (!id.empty() && !objectsById.emplace(id, created).second)
			throw std::runtime_error("Duplicate object id: " + id);
	}

	if (!world->player())
		throw std::runtime_error("A level must contain exactly one Mario");

	// resolve trigger references after all game objects have been created
	for (const auto& object : level.objects())
	{
		if (level.category(object) != "Trigger")
			continue;
		if (object.at("action") != "toggleFreezed")
			throw std::runtime_error("Unknown trigger action");
		std::string watchedName = object.at("watched");
		auto watchedIt = objectsById.find(watchedName);
		CollidableObject* watched = watchedIt == objectsById.end() ? nullptr : dynamic_cast<CollidableObject*>(watchedIt->second);
		if (!watched)
			throw std::runtime_error("Unknown trigger watched object: " + watchedName);
		const auto& targetNames = object.at("targets");
		if (!targetNames.is_array() || targetNames.empty())
			throw std::runtime_error("A trigger needs at least one target");
		std::vector<Object*> targets;
		for (const auto& targetName : targetNames)
		{
			auto target = objectsById.find(targetName.get<std::string>());
			if (target == objectsById.end())
				throw std::runtime_error("Unknown trigger target: " + targetName.get<std::string>());
			targets.push_back(target->second);
		}
		RotatedRectF geometry = LevelData::rotRect(object);
		if (geometry.angle != 0)
			throw std::runtime_error("CustomPlatformer requires axis-aligned rectangles");
		new Trigger(world.get(), geometry.toRect(), watched, [targets]()
			{
				for (auto* target : targets)
					target->toggleFreezed();
			});
	}

	return world.release();
}
