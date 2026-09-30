#include "Coin.h"
#include "SpriteFactory.h"
#include "Mario.h"
#include "Audio.h"
#include "Scene.h"
#include "HUD.h"
#include "PlatformerGame.h"

using namespace agp;

Coin::Coin(Scene* scene, const PointF& pos, int layer) :
	StaticObject(scene, RectF(pos.x, pos.y, 1, 1), SpriteFactory::instance()->get("coin"), layer)
{
	_compenetrable = true;
}

bool Coin::collision(CollidableObject* with, bool begin, Direction fromDir)
{
	Mario* mario = dynamic_cast<Mario*>(with);
	if (!mario)
		return false;

	dynamic_cast<PlatformerGame*>(Game::instance())->hud()->addCoin();

	kill();

	return true;
}

