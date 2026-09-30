#include "Enemy.h"
#include "Mario.h"
#include "Audio.h"
#include "Scene.h"
#include "StaticObject.h"
#include "KinematicObject.h"
#include "PlatformerGame.h"
#include "HUD.h"
#include "Points.h"

using namespace agp;

Enemy::Enemy(Scene* scene, const RectF& rect, Sprite* sprite, int layer)
	: DynamicObject(scene, rect, sprite, layer)
{
	_kickable = false;
	_stompable = true;
	_smashable = true;
	_stomped = false;
	_dying = false;
	_facingDir = Direction::LEFT;
}

void Enemy::score()
{
	int points_idx = dynamic_cast<PlatformerGame*>(Game::instance())->hud()->addKill();
	new Points(_scene, pos(), Points::Value(points_idx));
}

void Enemy::stomp(Mario* mario)
{
	if (!_stompable || _stomped)
		return;

	_stomped = true;
	Audio::instance()->playSound("Stomp");
	mario->bounce();
}

// implements behaviors common to all enemies
bool Enemy::collision(CollidableObject* with, bool begin, Direction fromDir)
{
	Mario* mario = dynamic_cast<Mario*>(with);
	Enemy* enemy = dynamic_cast<Enemy*>(with);
	StaticObject* staticObj = dynamic_cast<StaticObject*>(with);
	KinematicObject* kinObj = dynamic_cast<KinematicObject*>(with);

	if (begin && _smashable && mario && mario->invincible())
	{
		smash();
		return true;
	}
	else if (begin && mario && !mario->flashing() && fromDir != Direction::UP)
	{
		mario->powerdown();
		return true;
	}
	else if (begin && mario && !mario->flashing() && fromDir == Direction::UP)
	{
		stomp(mario);
		return true;
	}
	else if (begin && _xDir != Direction::NONE && (staticObj || enemy || kinObj) && (fromDir == Direction::RIGHT || fromDir == Direction::LEFT))
	{
		_xDir = inverse(_xDir);
		return true;
	}

	return false;
}

void Enemy::smash()
{
	if (_dying)
		return;

	score();
	_dying = true;
	_yGravityForce = 25;
	_vel.y = -8;
	_collidable = false;
	_flip = SDL_FLIP_VERTICAL;
	Audio::instance()->playSound("Kick");


	schedule("die-smash", 2, [this]() {kill(); });
}