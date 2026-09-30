#include "Goomba.h"
#include "SpriteFactory.h"
#include "Audio.h"
#include "Mario.h"
#include "GameScene.h"

using namespace agp;

Goomba::Goomba(Scene* scene, const PointF& pos, int layer) :
	Enemy(scene, RectF(pos.x, pos.y, 1.0f, 1.0f), nullptr, layer)
{
	_sprite = SpriteFactory::instance()->get("goomba_walk");
	_spriteStomped = SpriteFactory::instance()->get("goomba_stomped");

	_xDir = Direction::LEFT;
	_xVelMax = 2;
	_xMoveForce = 1000;

	_collider.adjust(0.2f, 0, -0.2f, -1 / 16.0f);
}

// @override: die if stomped
void Goomba::stomp(Mario* mario)
{
	if(!_dying && !_stomped)
		Enemy::stomp(mario);

	score();

	_dying = true;
	_xDir = Direction::NONE;
	_vel.x = 0;
	_sprite = _spriteStomped;
	_compenetrable = true;

	schedule("death", 2, [this]() {
		kill();
		});
}

// @override: +ignore collisions with dynamic objects if stomped
bool Goomba::collidableWith(CollidableObject* obj)
{
	if (_stomped && dynamic_cast<DynamicObject*>(obj))
		return false;
	else
		return Enemy::collidableWith(obj);
}