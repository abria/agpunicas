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
	_fit = false;
	_patrolArea = patrolRect;
	_currentWaypointIndex = 0;
	_state = State::PATROL;
	_target = _patrolArea.pos;
	_velMax = { 2, 2 };

	_collider.size.x = 0.8f;
	_collider.size.y = 0.8f;

	// animations
	for (int i = 0; i < 4; i++)
	{
		_sprites[i]["walk"] = SpriteFactory::instance()->get(std::string("soldier_" + dir2str(Direction(i))));
		_sprites[i]["attack"] = SpriteFactory::instance()->get(std::string("soldier_attack_" + dir2str(Direction(i))));
	}
	setSprite(_sprites[int(_facingDir)]["walk"]);

	_pathfinding = new Pathfinding(this, 50, 0.5);
}

void Soldier::changeState(State newState)
{
	if (newState == State::PATROL)
	{
		_target = _patrolArea.vertices()[(++_currentWaypointIndex) % 4];
		_velMax = { 2,2 };
	}
	else if (newState == State::CHASING)
	{
		Link* link = dynamic_cast<GameScene*>(_scene)->player()->to<Link*>();
		_target = _pathfinding->shortestPathNextTarget(link->rect().center());
		_velMax = { 3.5f, 3.5f };

		if (_target == PointF::invalid())	// hope to be luckier next time
		{
			changeState(State::SLEEP);
			return;
		}
	}
	else if (newState == State::SLEEP)
		_velMax = { 0,0 };
	_state = newState;
}

void Soldier::AI(bool targetReached)
{
	Link* link = dynamic_cast<GameScene*>(_scene)->player()->to<Link*>();
	float distanceFromLink = distance(link);
	float distanceToTrigger = 100;

	// if target reached while patrolling, keep patrolling
	if (_state == State::PATROL && targetReached)
		changeState(State::PATROL);

	// while patrolling, schedule random transition to sleep mode if link is far away
	else if (_state == State::PATROL && distanceFromLink >= distanceToTrigger)
		schedule("gosleep", float(1 + rand() % 10), [this]() { changeState(State::SLEEP); }, 0, false);

	// while patrolling, switch to chase mode if Link is close
	else if (_state == State::PATROL && distanceFromLink < distanceToTrigger)
		changeState(State::CHASING);

	// while sleeping, schedule random transition to patrol mode
	// or wake up immediately if close to Link
	else if (_state == State::SLEEP)
	{
		if (distanceFromLink > 5)
			schedule("wake", float(1 + rand() % 5), [this]() { changeState(State::PATROL); }, 0, false);
		else
		{
			unschedule("wake");
			changeState(State::CHASING);
		}
	}

	// if target reached while chasing, keep chasing
	else if (_state == State::CHASING && targetReached)
		changeState(State::CHASING);

	// while chasing, switch patrol mode if Link is far away
	else if (_state == State::CHASING && distance(link) > distanceToTrigger)
		changeState(State::PATROL);
}

void Soldier::update(float dt)
{
	Enemy::update(dt);
	
	// move towards target (y-first, then x)
	bool targetReached = false;
	if (std::fabs(_target.y - sceneCollider().center.y) > 0.1f)
	{
		//printf("move vertically: %s\n", dir2str(vec2dir(Vec2Df(0, _target.y - sceneCollider().center.y))).c_str());
		move(Direction::NONE, vec2dir(Vec2Df(0, _target.y - sceneCollider().center.y)));
	}
	else if (std::fabs(_target.x - sceneCollider().center.x) > 0.1f)
	{
		//printf("move horizontally\n");
		move(vec2dir(Vec2Df(_target.x - sceneCollider().center.x, 0), 0), Direction::NONE);
	}
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