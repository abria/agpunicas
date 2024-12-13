// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#include <queue>
#include "Pathfinding.h"
#include "DynamicObject.h"
#include "Scene.h"
#include "mathUtils.h"

using namespace agp;

Pathfinding::Pathfinding(DynamicObject* actor, float searchExtent, float cellSize)
{
	_actor = actor;
	_searchExtent = searchExtent;
	_cellSize = cellSize;
	_cellDims.x = int(ceil(_searchExtent / _cellSize));
	_cellDims.y = int(int(ceil(_searchExtent / _cellSize)));

	_cells.resize(_cellDims.y);
	for (int i = 0; i < _cells.size(); i++)
		_cells[i].resize(_cellDims.x);

	_initialized = false;
}

void Pathfinding::update()
{
	// clear visual grid
	for (auto r : _visualGrid)
		r->kill();
	_visualGrid.clear();

	// (re)center search area on actor
	_searchArea = RotatedRectF(_actor->sceneCollider().center, PointF(_searchExtent, _searchExtent), 0).toRect();

	// update free/occupied cells and centers
	int count_occupied = 0;
	int count_total = 0;
	for (int i = 0; i < _cells.size(); i++)
		for (int j = 0; j < _cells[i].size(); j++)
		{
			_cells[i][j].center.x = _searchArea.pos.x + j * _cellSize + _cellSize / 2;
			_cells[i][j].center.y = _searchArea.pos.y + i * _cellSize + _cellSize / 2;
			RectF collRect = _actor->collider().toRect();
			_cells[i][j].free = _actor->scene()->isEmpty(collRect - collRect.pos + _cells[i][j].center - collRect.size / 2);

			if (!_cells[i][j].free)
				count_occupied++;
			count_total++;

			// display grid
			/*RenderableObject* rend = new RenderableObject(_actor->scene(),
				RectF(_searchArea.pos.x + j * _cellSize,
					_searchArea.pos.y + i * _cellSize, _cellSize, _cellSize),
				_cells[i][j].free ? Color(0, 255, 0, 128) : Color(255, 0, 0, 128), 5);
			rend->setBorderColor(Color(0, 0, 0));
			_visualGrid.push_back(rend);*/
		}

	_initialized = true;
}

Point Pathfinding::nearestReachableCell(const Point& cell, const PointF& target)
{
	// init visits
	for (int i = 0; i < _cellDims.y; i++)
		for (int j = 0; j < _cellDims.x; j++)
			_cells[i][j].visited = false;

	Point nearestCell;
	float minDistance = agp::inf<float>();

	// BFS from cell, minimizing distance to target while visiting grid
	std::queue<Point> q;
	q.push(cell);
	_cells[cell.y][cell.x].visited = true;
	std::vector<Point> directions = { {-1,0},{1,0},{0,-1},{0,1} };
	while (!q.empty())
	{
		Point u = q.front();
		q.pop();

		for (auto& d : directions)
		{
			Point v = u + d;
			if (v.x >= 0 && v.y >= 0 && v.x < _cellDims.x && v.y < _cellDims.y &&
				_cells[v.y][v.x].free && !_cells[v.y][v.x].visited)
			{
				_cells[v.y][v.x].visited = true;
				q.push({ v.x, v.y });

				float dist = _cells[v.y][v.x].center.distance(target);
				if (dist < minDistance)
				{
					nearestCell = { v.x,v.y };
					minDistance = dist;
				}
			}
		}
	}

	return nearestCell;
}

PointF Pathfinding::shortestPathNextTarget(const PointF& dst)
{
	PointF src = _actor->sceneCollider().center;

	Point srcCell(int((src.x - _searchArea.pos.x) * _cellDims.x / _searchArea.size.x), int((src.y - _searchArea.pos.y) * _cellDims.y / _searchArea.size.y));
	bool frontierReached = (srcCell.x == _cellDims.x - 1) || (srcCell.y == _cellDims.y - 1) || (srcCell.x == 0) || (srcCell.y == 0);

	// regenerate grid if needed
	if (!_initialized || frontierReached)
		update();

	// pick as actual destination the closest-to-dst reachable free cell within search area
	srcCell = Point(int((src.x - _searchArea.pos.x) * _cellDims.x / _searchArea.size.x), int((src.y - _searchArea.pos.y) * _cellDims.y / _searchArea.size.y));
	Point dstCell = nearestReachableCell(srcCell, dst);

	// init distances
	for (int i = 0; i < _cellDims.y; i++)
		for (int j = 0; j < _cellDims.x; j++)
			_cells[i][j].distance = std::numeric_limits<int>::max();

	// BFS from dst to src with distance from dst computation
	std::queue<Point> q;
	q.push(dstCell);
	_cells[dstCell.y][dstCell.x].distance = 0;
	std::vector<Point> directions = { {-1,0},{1,0},{0,-1},{0,1} };
	while (!q.empty()) 
	{
		Point u = q.front();
		q.pop();

		for (auto& d : directions) 
		{
			Point v = u + d;
			if (v.x >= 0 && v.y >= 0 && v.x < _cellDims.x && v.y < _cellDims.y &&
				_cells[v.y][v.x].free && _cells[v.y][v.x].distance == std::numeric_limits<int>::max())
			{
				_cells[v.y][v.x].distance = _cells[u.y][u.x].distance + 1;
				q.push({ v.x, v.y });

				if (v == srcCell)
					break;
			}
		}
	}

	// select src neighbor with lower distance to dst, if any
	Point bestMove = Point::invalid();
	int bestDist = std::numeric_limits<int>::max();
	for (auto& d : directions) 
	{
		Point v = srcCell + d;
		if (v.x >= 0 && v.y >= 0 && v.x < _cellDims.x && v.y < _cellDims.y &&
			_cells[v.y][v.x].free && _cells[v.y][v.x].distance < bestDist) 
		{
			bestDist = _cells[v.y][v.x].distance;
			bestMove = v;
		}
	}

	if (bestMove == Point::invalid())
		return PointF::invalid();
	else
		return _cells[bestMove.y][bestMove.x].center;
}