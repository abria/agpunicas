#include "Spawnable.h"
#include "CoinSpawnable.h"
#include "Mushroom.h"
#include "Scene.h"
#include "GameScene.h"
#include "Mario.h"

using namespace agp;

Spawnable::Spawnable(Scene* scene, const RectF& rect, int layer) :
	DynamicObject(scene, rect, nullptr, layer)
{
	_bounceable = true;
}

Spawnable* Spawnable::spawn(Scene* scene, Spawnable::Type type, const PointF& pos, CollidableObject* spawner)
{
	if (type == Spawnable::Type::POWERUP)
	{
		if (dynamic_cast<Mario*>(dynamic_cast<GameScene*>(scene)->player())->super())
			return nullptr;	// @TODO spawn Leaf
		else
			return new Mushroom(scene, pos, false, spawner->layer() - 1);
	}
	else if (type == Spawnable::Type::LIFE)
		return new Mushroom(scene, pos, true, spawner->layer() - 1);
	else if (type == Spawnable::Type::COIN)
		return new CoinSpawnable(scene, pos + PointF((spawner->rect().size.x - 0.5f) / 2, 0), spawner->layer() - 1);
	else if(type == Spawnable::Type::STARMAN)
		return nullptr;		//@TODO spawn starman
	else
		return nullptr;
}