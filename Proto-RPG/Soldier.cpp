#include "Soldier.h"
#include "SpriteFactory.h"
#include "AnimatedSprite.h"
#include "RPGGameScene.h"
#include "Link.h"
#include <random>

using namespace agp;

Soldier::Soldier(Scene* scene, const PointF& pos, const RectF& patrolRect)
	: Enemy(scene, RectF(pos.x, pos.y, 1.625f, 2), nullptr, 1)
{
	_patrolArea = patrolRect;
	_currentWaypointIndex = 0;
	_state = State::PATROL;
	_target = _patrolArea.pos;
	_velMax = { 2, 2 };

	// animations
	for (int i = 0; i < 4; i++)
	{
		_sprites[i]["walk"] = SpriteFactory::instance()->get(std::string("soldier_" + dir2str(Direction(i))));
		_sprites[i]["attack"] = SpriteFactory::instance()->get(std::string("soldier_attack_" + dir2str(Direction(i))));
	}
	setSprite(_sprites[int(_facingDir)]["walk"]);
}

void Soldier::AI(bool targetReached)
{
	Link* link = dynamic_cast<GameScene*>(_scene)->player()->to<Link*>();
	float distanceFromLink = distance(link);

	// Finite-state-machine-based AI

	// while patrolling, go to next waypoint if target has been reached
	if (_state == State::PATROL && targetReached)
		_target = _patrolArea.vertices()[(++_currentWaypointIndex) % 4];

	// while patrolling, schedule random transition to sleep mode if link is far away
	else if (_state == State::PATROL && distanceFromLink >= 5)
	{
		schedule("gosleep", float(1 + rand() % 10), [this]()
			{
				_state = State::SLEEP;
				_velMax = { 0,0 };
			}, 0, false);
	}

	// while patrolling, switch to chase mode if Link is close
	else if (_state == State::PATROL && distanceFromLink < 5)
	{
		_state = State::CHASING;
		_target = link->rect().center();
		_velMax = { 3.5f, 3.5f };
	}

	// while sleeping, schedule random transition to patrol mode
	// or wake up immediately if close to Link
	else if (_state == State::SLEEP)
	{
		if (distanceFromLink > 5)
			schedule("wake", float(1 + rand() % 5), [this]()
				{
					_state = State::PATROL;
					_velMax = { 2,2 };
				}, 0, false);
		else
		{
			unschedule("wake");
			_state = State::CHASING;
			_target = link->rect().center();
			_velMax = { 3.5f, 3.5f };
		}
	}

	// while chasing, set current Link pos as new target
	else if (_state == State::CHASING && targetReached)
		_target = link->rect().center();

	// while chasing, switch patrol mode if Link is far away
	else if (_state == State::CHASING && distance(link) > 5)
	{
		_state = State::PATROL;
		_target = _patrolArea.vertices()[(_currentWaypointIndex) % 4];
		_velMax = { 2, 2 };
	}
}

void Soldier::update(float dt)
{
	Enemy::update(dt);
	
	// move towards target (y-first, then x)
	bool targetReached = false;
	if (std::fabs(_target.y - _rect.center().y) > 0.1f)
		move(Direction::NONE, vec2dir(Vec2Df(0, _target.y - _rect.center().y)));
	else if (std::fabs(_target.x - _rect.center().x) > 0.1f)
		move(vec2dir(Vec2Df(_target.x - _rect.center().x, 0), 0), Direction::NONE);
	else
		targetReached = true;

	// finite state machine
	AI(targetReached);

	// animations
	if (_state == State::CHASING)
		_sprite = _sprites[int(_facingDir)]["attack"];
	else
		_sprite = _sprites[int(_facingDir)]["walk"];

	dynamic_cast<AnimatedSprite*>(_sprite)->setPaused(_state == State::SLEEP);

	_flip = _facingDir == Direction::RIGHT ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}