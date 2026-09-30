#pragma once

#include "KinematicObject.h"
#include "Spawnable.h"

namespace agp
{
	class Block;
	class BrickDebris;
}

// Block that can items (coins, powerups, etc.)
class agp::Block : public KinematicObject
{
	public:

		enum class Type {QUESTION, BRICK, WOOD, EMPTY};

	protected:

		PointF _posOriginal;
		Block::Type _type;
		Sprite* _questionSprite;
		Sprite* _emptySprite;
		Sprite* _brickSprite;
		Sprite* _woodSprite;
		Spawnable::Type _spawnableType;

		// spawn spawnable contained in this block, if any
		virtual void spawnAbove();

		// smash enemies and bounce spawnables on top of the block, if any
		virtual void bumpAbove();

		// bounce when Mario hits from bottom
		virtual void bounce();

		// destroy (and spawn mini-bricks) when SuperMario hits from bottom
		virtual void breakBrick();

		virtual bool hasSpawnable() { return _spawnableType != Spawnable::Type::NONE; }

		void updateSprite();

	public:

		Block(
			Scene* scene, 
			const PointF& pos,
			Block::Type bType = Block::Type::QUESTION, 
			const std::string& sType = "Coin",
			int layer = 2);

		// +end spawn phase
		virtual void update(float dt) override;

		// +interaction with Mario
		// +start spawn phase
		virtual bool collision(CollidableObject* with, bool begin, Direction fromDir) override;

		virtual std::string name() override {
			return strprintf("Block[%d]", _id);
		}
};

// A brick block splits into four debris when hit by SuperMario
class agp::BrickDebris : public MovableObject
{
	public:

		BrickDebris(
			Scene* scene, 
			const PointF& pos, 
			const Vec2Df impulse, 
			int layer = 2);

		virtual std::string name() override {
			return strprintf("BrickDebris[%d]", _id);
		}
};