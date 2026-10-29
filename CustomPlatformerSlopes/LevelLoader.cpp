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
#include <stdexcept>

using namespace agp;

namespace
{
	RectF axisAlignedRect(const nlohmann::ordered_json& object)
	{
		RotatedRectF geometry = LevelData::rotRect(object);
		if (geometry.angle != 0 || geometry.yUp)
			throw std::runtime_error("CustomPlatformerSlopes requires axis-aligned rectangles with yUp=false");
		return geometry.toRect();
	}
}

LevelLoader::LevelLoader() {}

void LevelLoader::loadJson(PlatformerGameScene* world, const LevelData& level)
{
	level.configure(world);
	if (world->rect().yUp)
		throw std::runtime_error("CustomPlatformerSlopes requires a scene with yUp=false");

	// Fixed level graphics; the editor places collision shapes over this image.
	world->addBackgroundImage(new RenderableObject(world,
		RectF(0, -25, 176, 41), SpriteFactory::instance()->get("level_1_1"), 0));

	for (const auto& object : level.objects())
	{
		std::string category = level.category(object);
		int layer = object.value("layer", 1);
		if (category == "Platform")
		{
			auto points = LevelData::multiline(object.at("multiline"));
			for (size_t i = 1; i < points.size(); ++i)
			{
				LineF line(points[i - 1], points[i]);
				if (!line.isValid())
					continue;
				if (line.start.y != line.end.y)
					throw std::runtime_error("Platform segments must be horizontal; use Slope for ramps");
				new Platform(world, line, layer);
			}
			continue;
		}

		if (category == "Slope")
		{
			if (!object.contains("multiline"))
				throw std::runtime_error("Slope requires a multiline of inclined segments");
			auto points = LevelData::multiline(object.at("multiline"));
			Color color = LevelData::color(object);
			for (size_t i = 1; i < points.size(); ++i)
			{
				PointF left = points[i - 1], right = points[i];
				if (left.x > right.x)
					std::swap(left, right);
				if (left.x == right.x || left.y == right.y)
					throw std::runtime_error("Slope segments must be inclined, neither horizontal nor vertical");
				// The line determines the direction; the rectangle is only its runtime bounds.
				new Slope(world, LineF(left, right).boundingRect(false), color, right.y < left.y, layer);
			}
			continue;
		}

		RectF rect = axisAlignedRect(object);
		Object* created = nullptr;
		if (category == "Static" || category == "Pipe")
			created = new StaticObject(world, rect, layer);
		else if (category == "Block" || category == "Brick" || category == "Wood")
		{
			std::string content = object.value("content", "");
			if (!content.empty() && content != "coin" && content != "powerup" && content != "life" && content != "starman")
				throw std::runtime_error("Unknown block content: " + content);
			Block::Type type = category == "Block" ? Block::Type::QUESTION :
				(category == "Brick" ? Block::Type::BRICK : Block::Type::WOOD);
			created = new Block(world, rect.pos, type, content, layer);
		}
		else if (category == "Coin")
			created = new Coin(world, rect.pos, layer);
		else if (category == "Goomba")
			created = new Goomba(world, rect.pos, layer);
		else if (category == "Lift")
		{
			float range = object.value("range", 3.0f);
			if (!std::isfinite(range) || range <= 0)
				throw std::runtime_error("Lift range must be positive");
			created = new Lift(world, rect, object.value("vertical", true), range, layer);
		}
		else if (category == "Mario")
		{
			if (world->player())
				throw std::runtime_error("A level must contain exactly one Mario");
			created = new Mario(world, rect.pos - PointF(1 / 16.0f, 0));
			world->setPlayer(created);
		}
		else
			throw std::runtime_error("Unknown CustomPlatformerSlopes category: " + category);
		created->setLayer(layer);
	}

	if (!world->player())
		throw std::runtime_error("A level must contain exactly one Mario");

	// The death trigger remains a C++ gameplay rule, as in the original SMB3.
	Mario* mario = world->player()->to<Mario*>();
	new Trigger(world, RectF(world->rect().left(), 4, world->rect().size.x, 0.1f), mario,
		[mario]() { mario->die(); });
}

Scene* LevelLoader::load(const std::string& name)
{
	LevelData level(std::string(SDL_GetBasePath()) + "levels/" + name + ".json");
	auto* world = new PlatformerGameScene(level.sceneRect(), level.pixelUnitSize(), level.timeStep());
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
