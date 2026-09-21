// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "Enemy.h"
#include "Pathfinding.h"

namespace agp
{
	class Soldier;
	class Link;
}

// Soldier class
// - provides basic patrolling movement around a rectangle path
class agp::Soldier : public Enemy
{
	public:

		enum class State { PATROL, SLEEP, CHASING };

	protected:

		State _state;
		RectF _patrolArea;
		int _currentWaypointIndex;
		PointF _target;
		std::map<std::string, Sprite*> _sprites[4];
		Pathfinding* _pathfinding;

		// finite state machine AI
		void AI(bool targetReached);
		void changeState(State newState);

	public:

		Soldier(Scene* scene, const PointF& pos, const RectF& patrolRect);

		// extends game logic (+Soldier logic)
		virtual void update(float dt) override;

		virtual std::string name() override { return strprintf("Soldier[%d]", _id); }

		//virtual bool collidableWith(CollidableObject* obj) override { return false; }
};