// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>
#include "GameScene.h"

namespace agp
{
	struct Cell;
	class Grid;
}

// Grid class
// - graph (grid) representation of game scene
// - provides basic pathfinding
class agp::Grid
{
	protected:

		GameScene* _gameScene;
		float _cellSize = 1;		// in scene units
		std::vector < std::vector <Cell> > _cells;
		Point _cellDims;
		bool _initialized;

	public:

		Grid(GameScene* gameScene, float cellSize = 1);

		virtual void setFree(const RectF& region, bool free);

		virtual void syncWithScene();

		virtual PointF BFSPathFinding(const PointF& src, const PointF& dst);
};

struct agp::Cell
{
	bool free;
	Vec2Df center;
	bool visited;
	int distance;

	Cell() : free(false), center(0, 0) { reset(); }
	
	void reset() 
	{
		visited = false;
		distance = std::numeric_limits<int>::max();
	}
};
