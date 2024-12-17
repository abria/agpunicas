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
#include "timeUtils.h"
#include "StaticObject.h"

using namespace agp;

Pathfinding::Pathfinding(DynamicObject* actor, float searchExtent, float cellSize)
{
	_actor = actor;
	_searchExtent = searchExtent;
	_cellSize = cellSize;
	_searchArea = actor->scene()->rect();
	_cellDims.x = int(ceil(_searchArea.size.x / _cellSize));
	_cellDims.y = int(int(ceil(_searchArea.size.y / _cellSize)));

	_cells.resize(_cellDims.y);
	for (int i = 0; i < _cells.size(); i++)
	{
		_cells[i].resize(_cellDims.x);
		for (int j = 0; j < _cells[i].size(); j++)
		{
			_cells[i][j].center.x = _searchArea.pos.x + j * _cellSize + _cellSize / 2;
			_cells[i][j].center.y = _searchArea.pos.y + i * _cellSize + _cellSize / 2;
		}
	}

	_initialized = false;
}

void Pathfinding::update()
{
	Timer<float> timer;

	bool debug = false;

	// center search area on actor
	RectF actorSearchArea = RotatedRectF(_actor->sceneCollider().center, PointF(_searchExtent, _searchExtent), 0).toRect();
	Point actorStartCell(int(actorSearchArea.pos.x * _cellDims.x / _searchArea.size.x), int(actorSearchArea.pos.y * _cellDims.y / _searchArea.size.y));
	Point actorEndCell(int(actorSearchArea.br().x * _cellDims.x / _searchArea.size.x), int(actorSearchArea.br().y * _cellDims.y / _searchArea.size.y));
	actorStartCell.x = (std::max)(actorStartCell.x, 0);
	actorStartCell.y = (std::max)(actorStartCell.y, 0);
	actorEndCell.x = (std::min)(actorEndCell.x, _cellDims.x - 1);
	actorEndCell.y = (std::min)(actorEndCell.y, _cellDims.y - 1);

	// get actor collider, it will be used to check for free/occupied cells
	RectF actorRect = _actor->collider().toRect();

	// first (one-time) initialization of _searchExtent x _searchExtent centered on actor
	// Fast approach: only query cells within scene's static objects AABBs
	if (!_initialized)
	{
		for (int i = actorStartCell.y; i <= actorEndCell.y; i++)
			for (int j = actorStartCell.x; j <= actorEndCell.x; j++)
			{
				_cells[i][j].free = true;
				_cells[i][j].synced = true;
			}

		auto allObjects = _actor->scene()->objects();
		for (auto obj : allObjects)
		{
			if (obj->to<StaticObject*>())
			{
				RectF objAABB = obj->to<StaticObject*>()->sceneCollider().boundingRect();

				RectF expandedAABB;
				expandedAABB.pos = objAABB.pos - actorRect.size / 2;
				expandedAABB.size = objAABB.size + actorRect.size;

				Point startCell(int(expandedAABB.pos.x * _cellDims.x / _searchArea.size.x), int(expandedAABB.pos.y * _cellDims.y / _searchArea.size.y));
				Point endCell(int(expandedAABB.br().x * _cellDims.x / _searchArea.size.x), int(expandedAABB.br().y * _cellDims.y / _searchArea.size.y));
				startCell.x = (std::max)(startCell.x, actorStartCell.x);
				startCell.y = (std::max)(startCell.y, actorStartCell.y);
				endCell.x = (std::min)(endCell.x, actorEndCell.x);
				endCell.y = (std::min)(endCell.y, actorEndCell.y);

				for (int i = startCell.y; i <= endCell.y; i++)
					for (int j = startCell.x; j <= endCell.x; j++)
						if (_cells[i][j].free)
							_cells[i][j].free = _actor->scene()->isEmpty(actorRect - actorRect.pos + _cells[i][j].center - actorRect.size / 2);
			}
		}
		_initialized = true;

		if (debug)
		{
			for (int i = actorStartCell.y; i <= actorEndCell.y; i++)
				for (int j = actorStartCell.x; j <= actorEndCell.x; j++)
				{
					RenderableObject* rend = new RenderableObject(_actor->scene(),
						RectF(_searchArea.pos.x + j * _cellSize,
							_searchArea.pos.y + i * _cellSize, _cellSize, _cellSize),
						_cells[i][j].free ? Color(0, 255, 0, 90) : Color(255, 0, 0, 90), 5);
					rend->setBorderColor(Color(0, 0, 0, 90));
					_visualGrid.push_back(rend);
				}
		}
	}
	// update during exploration
	else
	{
		for (int i = actorStartCell.y; i <= actorEndCell.y; i++)
			for (int j = actorStartCell.x; j <= actorEndCell.x; j++)
				if (_cells[i][j].synced == false)
				{
					_cells[i][j].free = _actor->scene()->isEmpty(actorRect - actorRect.pos + _cells[i][j].center - actorRect.size / 2);
					_cells[i][j].synced = true;

					if (debug)
					{
						RenderableObject* rend = new RenderableObject(_actor->scene(),
							RectF(_searchArea.pos.x + j * _cellSize,
								_searchArea.pos.y + i * _cellSize, _cellSize, _cellSize),
							_cells[i][j].free ? Color(0, 255, 0, 90) : Color(255, 0, 0, 90), 5);
						rend->setBorderColor(Color(0, 0, 0, 90));
						_visualGrid.push_back(rend);
					}
				}
	}

	printf("Pathfinding grid updated in %.0f ms\n", timer.elapsed() * 1000);
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
				_cells[v.y][v.x].free && !_cells[v.y][v.x].visited && _cells[v.y][v.x].synced)
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
	
	// update grid around actor
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
				_cells[v.y][v.x].free && _cells[v.y][v.x].distance == std::numeric_limits<int>::max() 
				&& _cells[v.y][v.x].synced)
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