#include "Enemy.h"
#include "Link.h"
#include "Audio.h"
#include "Scene.h"
#include "Sword.h"

using namespace agp;

Enemy::Enemy(Scene* scene, const RotatedRectF& rrect, Sprite* sprite, int layer)
	: DynamicObject(scene, rrect, sprite, layer)
{
}

bool Enemy::collision(CollidableObject* with, bool begin, const Vec2Df& normal)
{
	DynamicObject::collision(with, begin, normal);

	if (with->to<Sword*>())
		_scene->killObject(this);

	return false;
}