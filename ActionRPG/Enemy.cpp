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

bool Enemy::collision(CollidableObject* with, Contact contact, const Vec2Df& normal)
{
	DynamicObject::collision(with, contact, normal);

	if (contact == Contact::BEGIN && with->to<Sword*>())
		kill();

	return false;
}