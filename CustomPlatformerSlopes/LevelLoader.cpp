// From "Algorithms and Game Programming" in C++ by Alessandro Bria.
// Released under the BSD License; see LICENSE in the repository root.

#include "LevelLoader.h"
#include "LevelData.h"
#include "SpriteFactory.h"
#include "PlatformerGameScene.h"
#include "Mario.h"
#include "Goomba.h"
#include "Block.h"
#include "Coin.h"
#include "Platform.h"
#include "Slope.h"
#include "Lift.h"
#include "Trigger.h"
#include <string>

using namespace agp;

void LevelLoader::loadJson(PlatformerGameScene* world, const LevelData& level)
{
	if (world->rect().yUp)
		throw "CustomPlatformerSlopes requires a scene with yUp=false";

	// fixed level graphics: the editor places collision shapes over this image.
	world->addBackgroundImage(new RenderableObject(world,
		RectF(0, -25, 176, 41), SpriteFactory::instance()->get("level_1_1"), 0));

	for (const auto& object : level.objects())
	{
		std::string category = level.category(object);
		if (category == "Platform" || category == "Slope")
		{
			auto points = LevelData::multiline(object.at("multiline"));
			for (size_t i = 1; i < points.size(); ++i)
				if (category == "Platform")
					new Platform(world, LineF(points[i - 1], points[i]));
				else
					new Slope(world, LineF(points[i - 1], points[i]));
			continue;
		}

		RectF rect = LevelData::axisAlignedRect(object);
		if (category == "Static" || category == "Pipe")
			new StaticObject(world, rect);
		else if (category == "Block" || category == "Brick" || category == "Wood")
		{
			std::string content = object.value("content", "");
			if (!content.empty() && content != "coin" && content != "powerup" && content != "life" && content != "starman")
				throw "Unknown block content";
			Block::Type type = category == "Block" ? Block::Type::QUESTION :
				(category == "Brick" ? Block::Type::BRICK : Block::Type::WOOD);
			new Block(world, rect.pos, type, content);
		}
		else if (category == "Coin")
			new Coin(world, rect.pos);
		else if (category == "Goomba")
			new Goomba(world, rect.pos);
		else if (category == "Lift")
		{
			float range = object.value("range", 3.0f);
			if (!std::isfinite(range) || range <= 0)
				throw "Lift range must be positive";
			new Lift(world, rect, object.value("vertical", true), range);
		}
		else if (category == "Mario")
		{
			if (world->player())
				throw "A level must contain exactly one Mario";
			world->setPlayer(new Mario(world, rect.pos - PointF(1 / 16.0f, 0)));
		}
		else
			throw "Unknown CustomPlatformerSlopes category";
	}

	if (!world->player())
		throw "A level must contain exactly one Mario";

	// death trigger
	Mario* mario = world->player()->to<Mario*>();
	new Trigger(world, RectF(world->rect().left(), 4, world->rect().size.x, 0.1f), mario,
		[mario]() { mario->die(); });
}

Scene* LevelLoader::load(const std::string& name)
{
	std::string path = std::string(SDL_GetBasePath()) + "levels/" + name + ".json";
	LevelData level(path);
	auto* world = new PlatformerGameScene(RectF(0, -20, 176, 50), {16, 16}, 0.01f, {156, 252, 240, 255});
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
