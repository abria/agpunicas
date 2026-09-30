#include "CoinSpawnable.h"
#include "SpriteFactory.h"
#include "Audio.h"
#include "PlatformerGame.h"
#include "HUD.h"

using namespace agp;

CoinSpawnable::CoinSpawnable(Scene* scene, const PointF& pos, int layer) :
	Spawnable(scene, RectF(pos.x, pos.y, 0.5f, 1.0f), layer)
{
	_sprite = SpriteFactory::instance()->get("spawnable_coin");

	_collidable = false;

	_yVelMax = 30;
	velAdd(Vec2Df(0, -_yVelMax));

	Audio::instance()->playSound("Coin");

	dynamic_cast<PlatformerGame*>(Game::instance())->hud()->addCoin();

	schedule("die", 0.5f, [this]() {
		kill();
		});
}