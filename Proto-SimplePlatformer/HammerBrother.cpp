#include "HammerBrother.h"
#include "SpriteFactory.h"
#include "Hammer.h"
#include "PlatformerGameScene.h"
#include "Mario.h"
#include <random>

using namespace agp;

HammerBrother::HammerBrother(Scene* scene, const PointF& pos)
	: Enemy(scene, RectF(pos.x + 1 / 16.0f, pos.y - 1, 1, 2), nullptr)
{
	_collider.adjust(0.1f, 0.4f, -0.1f, -1 / 16.0f);

	_sprites["walk"] = SpriteFactory::instance()->get("hammer_brother_walk");
	_sprites["throw"] = SpriteFactory::instance()->get("hammer_brother_throw");
	_sprites["jump"] = SpriteFactory::instance()->get("hammer_brother_jump");
	_sprite = _sprites["throw"];
	_pivot = pos;
	_throwing = false;
	_chasing = false;

	// default physics
	_yGravityForce = 25;
	_xMoveForce = 1000;
	_xFrictionForce = 0;
	_xSkiddingForce = 1000;
	_xVelMax = 1;
	_xDir = Direction::LEFT;
	_halfRangeX = 0.7f;

	// scripting (hammer spawn loop)
	schedule("spawnHammer", 0.7f + (rand()%100)/1000.0f, [this]
		{
			if (rand() % 5 > 0 && !_dying)
			{
				new Hammer(_scene, _rect.tl(), this);
				_throwing = true;
				schedule("throwing_off", 0.5f, [this]() {_throwing = false; });
			}
		}, -1);

	// scripting (jump loop)
	schedule("jump", 1.0f, [this]
		{
			if (!midair() && rand() % 2 && !_dying)		// 50% probability of jump
			{
				if (rand() % 2)					// 50% probability full jump
				{
					velAdd(Vec2Df(0, -_yJumpImpulse));
					_collidable = false; 
					schedule("collidable_on", 0.5f, [this]() {_collidable = true; });
				}
				else if (_rect.bottom() < 0)	// 50% probability small jump (if feasible)
				{
					velAdd(Vec2Df(0, -5));
					_collidable = false;
					schedule("collidable_on", 0.5f, [this]() {_collidable = true; });
				}
			}
		}, -1);

	// scripting (chasing Mario)
	schedule("chasing", 15.0f + rand() % 10, [this]
		{
			_chasing = true;
			_xVelMax *= 2;
		});
}

void HammerBrother::update(float dt)
{
	Enemy::update(dt);

	Mario* mario = dynamic_cast<Mario*>(dynamic_cast<PlatformerGameScene*>(_scene)->player());

	//_facingDir = Direction::NONE;
	//_x_dir = Direction::NONE;
	//_vel.x = 0;
	//return;

	// state changes logic
	if(mario->rect().center().x > _rect.center().x)
		_facingDir = Direction::RIGHT;
	else
		_facingDir = Direction::LEFT;
	
	// horizontal movement
	if (_chasing)
		_xDir = _facingDir;
	else if (_rect.center().x >= _pivot.x + _halfRangeX)
		_xDir = Direction::LEFT;
	else if(_rect.center().x <= _pivot.x - _halfRangeX)
		_xDir = Direction::RIGHT;

	// animations
	if(!_dying)
		_flip = _facingDir == Direction::RIGHT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	if (!_throwing && midair())
		_sprite = _sprites["jump"];
	else if(_throwing)
		_sprite = _sprites["throw"];
	else 
		_sprite = _sprites["walk"];
}