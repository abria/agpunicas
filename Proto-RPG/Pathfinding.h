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
#include "geometryUtils.h"

namespace agp
{
	struct Cell;
	class Pathfinding;
	class DynamicObject;
	class RenderableObject;
}

// Pathfinding class
// - provides basic (nonoptimal) pathfinding via grid-based BFS
class agp::Pathfinding
{
	protected:

		DynamicObject* _actor;		// who follows the path
		float _searchExtent;		// size of search square, in scene units
		RectF _searchArea;			// search area location in game scene
		float _cellSize = 1;		// search granularity, in scene units
		std::vector < std::vector <Cell> > _cells;
		Point _cellDims;
		bool _initialized;

		std::vector<RenderableObject*> _visualGrid;

		virtual Point nearestReachableCell(const Point& cell, const PointF& target);
	
	public:

		Pathfinding(DynamicObject* actor, float searchExtent = 25, float cellSize = 1);

		virtual void update();
		virtual PointF shortestPathNextTarget(const PointF& dst);
};

struct agp::Cell
{
	bool free;
	Vec2Df center;
	bool visited;
	int distance;

	Cell() : free(false), center(0, 0), visited(false), distance(std::numeric_limits<int>::max()) {}
};
