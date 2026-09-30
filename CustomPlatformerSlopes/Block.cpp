#include "Block.h"
#include "SpriteFactory.h"
#include "Mario.h"
#include "Audio.h"
#include "Scene.h"
#include "Enemy.h"
#include <algorithm>

using namespace agp;

Block::Block(
	Scene* scene, 
	const PointF& pos, 
	Block::Type bType,
	const std::string& sType, 
	int layer) :
	KinematicObject(scene, RectF(pos.x, pos.y, 1, 1), nullptr, layer)
{
	_type = bType;
	
	std::string spawnType = sType;
	std::transform(spawnType.begin(), spawnType.end(), spawnType.begin(), [](unsigned char c) { return std::tolower(c); });
	if (spawnType == "powerup")
		_spawnableType = Spawnable::Type::POWERUP;
	else if (spawnType == "life")
		_spawnableType = Spawnable::Type::LIFE;
	else if (spawnType == "starman")
		_spawnableType = Spawnable::Type::STARMAN;
	else if (spawnType == "coin")
		_spawnableType = Spawnable::Type::COIN;
	else
		_spawnableType = Spawnable::Type::NONE;

	_posOriginal = pos;
	_yGravityForce = 0;

	_questionSprite = SpriteFactory::instance()->get("block_question");
	_emptySprite = SpriteFactory::instance()->get("block_empty");
	_brickSprite = SpriteFactory::instance()->get("brick");
	_woodSprite = SpriteFactory::instance()->get("wood");

	// hide level background
	new RenderableObject(scene, rect(), scene->backgroundColor(), 1);
}

void Block::updateSprite()
{
	if (_type == Block::Type::QUESTION)
		_sprite = _questionSprite;
	else if (_type == Block::Type::BRICK)
		_sprite = _brickSprite;
	else if (_type == Block::Type::WOOD)
		_sprite = _woodSprite;
	else
		_sprite = _emptySprite;
}

void Block::update(float dt)
{
	KinematicObject::update(dt);

	// restore original position when spawn phase ends
	if (pos().y >= _posOriginal.y)
	{
		_vel.y = 0;
		_yGravityForce = 0;
		setPos(_posOriginal);
	}

	updateSprite();
}

bool Block::collision(CollidableObject* with, bool begin, Direction fromDir)
{
	// interactions with Mario
	Mario* mario = dynamic_cast<Mario*>(with);
	if (mario && fromDir == Direction::DOWN)
	{
		if(_type == Block::Type::QUESTION || _type == Block::Type::BRICK)
		{
			if (_type == Block::Type::BRICK && !hasSpawnable() && mario->super())
				breakBrick();
			else
			{
				Audio::instance()->playSound("Bump");
				bounce();
				
				if (hasSpawnable())
					spawnAbove();
			}

			bumpAbove();
		}

		if (_type == Block::Type::QUESTION || (_type == Block::Type::BRICK && hasSpawnable()))
			_type = Block::Type::EMPTY;
	}

	// if not moving, behave like a static object
	if (_prevVel == Vec2Df(0, 0) && _vel == Vec2Df(0, 0))
		return true;
	// otherwise like a kinematic
	else
		return KinematicObject::collision(with, begin, fromDir);
}

void Block::spawnAbove()
{
	if (_spawnableType != Spawnable::Type::NONE)
		Spawnable::spawn(_scene, _spawnableType, pos(), this);
}

void Block::bumpAbove()
{
	Objects objs = _scene->raycast(LineF(rect().tl() + PointF(+0.1f, -0.1f), rect().tr() + PointF(-0.1f, -0.1f)));
	for (auto obj : objs)
	{
		Enemy* enemy = dynamic_cast<Enemy*>(obj);
		Spawnable* spawnable = dynamic_cast<Spawnable*>(obj);
		if (enemy && enemy->smashable())
			enemy->smash();
		else if (spawnable && spawnable->bounceable())
			spawnable->jump();
	}
}

void Block::bounce()
{
	_yGravityForce = 100;
	velAdd(Vec2Df(0, -_yJumpImpulse * 0.8f));
}

void Block::breakBrick()
{
	if (_killed)
		return;

	Audio::instance()->playSound("Block Break");
	
	kill();
	_visible = false;

	new BrickDebris(_scene, rect().tl(), Vec2Df(-5, -15));
	new BrickDebris(_scene, rect().tr(), Vec2Df( 5, -15));
	new BrickDebris(_scene, rect().bl(), Vec2Df(-5, -10));
	new BrickDebris(_scene, rect().br(), Vec2Df( 5, -10));
}

BrickDebris::BrickDebris(
	Scene* scene,
	const PointF& pos,
	const Vec2Df impulse,
	int layer) : 
	MovableObject(
		scene, 
		RectF(0, 0, 0.5f, 1).centerOn(pos),
		SpriteFactory::instance()->get("brick_debris"), layer)
{
	_vel = impulse;
	_xFrictionForce *= 0.8f;

	schedule("death", 2, [this]() {
		kill();
		});
}

