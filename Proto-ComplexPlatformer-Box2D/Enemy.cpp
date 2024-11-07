#include "Enemy.h"
#include "Player.h"
#include "Audio.h"
#include "Scene.h"
#include "Fire.h"

using namespace agp;

Enemy::Enemy(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer)
	: DynamicObject(scene, obb, sprite, layer)
{
	_smashable = true;
	_dying = false;
}

void Enemy::collision(RigidObject* with, bool begin, const Vec2Df& normal, b2ShapeId& shapeA, b2ShapeId& shapeB)
{
	Player* player = dynamic_cast<Player*>(with);

	Fire* fire = dynamic_cast<Fire*>(with);
	if (fire)
	{
		hurt();
		_scene->killObject(fire);
	}
	//if (player)
	//	hurt();
}

void Enemy::update(float dt)
{
	DynamicObject::update(dt);

	if (_dying)
		_obb.size -= 5 * dt;
}

void Enemy::hurt()
{
	_dying = true;
	b2Body_Disable(_bodyId);
	Audio::instance()->playSound("kick");

	schedule("die-hurt", 1, [this]() {_scene->killObject(this); });
}