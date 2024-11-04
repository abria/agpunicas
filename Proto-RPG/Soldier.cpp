#include "Soldier.h"
#include "SpriteFactory.h"
#include "RPGGameScene.h"
#include "Link.h"
#include <random>

using namespace agp;

Soldier::Soldier(Scene* scene, const PointF& pos, const RectF& patrolRect)
	: Enemy(scene, RectF(pos.x, pos.y, 1.625f, 2), nullptr, 1)
{
	_patrolRect = patrolRect;
	_state = State::REACHSTART;
	_nextPoint = _patrolRect.pos;
	printf("nextPoint = %s\n", _nextPoint.str().c_str());

	// animations
	for (int i = 0; i < 4; i++)
	{
		_sprites[i]["walk"] = SpriteFactory::instance()->get(std::string("soldier_" + dir2str(Direction(i))));
		_sprites[i]["attack"] = SpriteFactory::instance()->get(std::string("soldier_attack_" + dir2str(Direction(i))));
	}
	setSprite(_sprites[int(_facingDir)]["walk"]);
}

void Soldier::update(float dt)
{
	Enemy::update(dt);

	//printf("%s\n", _rect.str().c_str());

	// reach next point
	bool nextPointReached = false;
	if (std::fabs(_nextPoint.y - _rect.pos.y) > 0.1)
	{
		//printf("Y move %f\n", Vec2Df(0, _nextPoint.y - _rect.pos.y).versY());
		move(Direction::NONE, normal2dir(Vec2Df(0, Vec2Df(0, _nextPoint.y - _rect.pos.y).versY())));
	}
	else if (std::fabs(_nextPoint.x - _rect.pos.x) > 0.1)
	{
		//printf("X move %f\n", Vec2Df(_nextPoint.x - _rect.pos.x, 0).versX());
		move(normal2dir(Vec2Df(Vec2Df(_nextPoint.x - _rect.pos.x, 0).versX(), 0)), Direction::NONE);
	}
	else
		nextPointReached = true;

	// state logic
	if (nextPointReached)
	{
		printf("\nnextPointReached\n");
		static int index = 0;
		if (_state == State::REACHSTART)
			_state = State::PATROL;
		else if (_state == State::PATROL)
		{
			_nextPoint = _patrolRect.vertices()[(++index) % 4];
			printf("nextPoint = %s\n", _nextPoint.str().c_str());
		}
	}

	// animations
	if (_state == State::CHASING)
		_sprite = _sprites[int(_facingDir)]["attack"];
	else
		_sprite = _sprites[int(_facingDir)]["walk"];
	_flip = _facingDir == Direction::RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}