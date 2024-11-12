#include "Enemy.h"
#include "Link.h"
#include "Audio.h"
#include "Scene.h"
#include "Sword.h"

using namespace agp;

Enemy::Enemy(Scene* scene, const RectF& rect, Sprite* sprite, int layer)
	: DynamicObject(scene, rect, sprite, layer)
{
}

bool Enemy::collision(CollidableObject* with, Direction fromDir)
{
	DynamicObject::collision(with, fromDir);

	if (with->to<Sword*>())
		_scene->killObject(this);

	return false;
}