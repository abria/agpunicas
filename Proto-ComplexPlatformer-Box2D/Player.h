#pragma once

#include "DynamicObject.h"

namespace agp
{
	class Terrain;
	class Sprite;
	class Player;
}

class agp::Player : public DynamicObject
{
	private:

		bool _running;
		bool _dashing;
		Direction _lastNonZeroXDir;
		float _xDashForce;

		std::map<std::string, Sprite*> _sprites;

	public:

		Player(GameScene* scene, PointF pos);

		void setRunning(bool running);

		// override DynamicObject's update (+slope force correction)
		virtual void update(float dt) override;

		// override DynamicObject's move (+lastNonZeroXDir)
		virtual void move(Direction dir) override;

		virtual void dash();

		virtual std::string name() override { return strprintf("Player[%d]", _id); }
};