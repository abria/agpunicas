#include "Enemy.h"
#include "Link.h"
#include "Audio.h"
#include "Scene.h"

using namespace agp;

Enemy::Enemy(Scene* scene, const RectF& rect, Sprite* sprite, int layer)
	: DynamicObject(scene, rect, sprite, layer)
{
}

bool Enemy::collision(CollidableObject* with, Direction fromDir)
{
	DynamicObject::collision(with, fromDir);

	return false;
}