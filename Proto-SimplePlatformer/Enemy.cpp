#include "Enemy.h"
#include "Mario.h"
#include "Audio.h"
#include "Scene.h"

using namespace agp;

Enemy::Enemy(Scene* scene, const RectF& rect, Sprite* sprite, int layer)
	: DynamicObject(scene, rect, sprite, layer)
{
	_smashable = true;
	_dying = false;
	_facingDir = Direction::LEFT;
}

bool Enemy::collision(CollidableObject* with, bool begin, Direction fromDir)
{
	Mario* mario = dynamic_cast<Mario*>(with);

	if (mario)
	{
		if (_smashable && mario->invincible())
			smash();
		else
			mario->hurt();

		return true;
	}

	return false;
}

void Enemy::smash()
{
	_dying = true;
	_yGravityForce = 25;
	_vel.y = -8;
	_collidable = false;
	_flip = SDL_FLIP_VERTICAL;
	Audio::instance()->playSound("kick");

	schedule("die-smash", 2, [this]() {_scene->killObject(this); });
}