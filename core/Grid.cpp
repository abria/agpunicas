// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#include <queue>
#include "Grid.h"

using namespace agp;

Grid::Grid(GameScene* gameScene, float cellSize)
{
	_gameScene = gameScene;
	_cellSize = cellSize;
	_cellDims.x = int(ceil(_gameScene->rect().size.x / _cellSize));
	_cellDims.y = int(int(ceil(_gameScene->rect().size.y / _cellSize)));

	_cells.resize(_cellDims.y);
	for (int i = 0; i < _cells.size(); i++)
		_cells[i].resize(_cellDims.x);

	_initialized = false;
}

void Grid::syncWithScene()
{
	RectF sceneRect = _gameScene->rect();

	int count = 0;
	for (int i = 0; i < _cells.size(); i++)
		for (int j = 0; j < _cells[i].size(); j++)
		{
			_cells[i][j].center.x = sceneRect.pos.x + j * _cellSize + _cellSize / 2;
			_cells[i][j].center.y = sceneRect.pos.y + i * _cellSize + _cellSize / 2;
			_cells[i][j].free = _gameScene->isEmpty(RectF(sceneRect.pos.x + j * _cellSize, sceneRect.pos.y + i * _cellSize, _cellSize, _cellSize, sceneRect.yUp));
			if (!_cells[i][j].free)
				count++;
		}

	printf("#occupate = %d\n", count);

	_initialized = true;
}

void Grid::setFree(const RectF& region, bool free)
{
	//@TODO
}

PointF Grid::BFSPathFinding(const PointF& src, const PointF& dst)
{
	if (!_initialized)
		syncWithScene();

	Point srcCell(int(src.x * _cellDims.x / _gameScene->rect().size.x), int(src.y * _cellDims.y / _gameScene->rect().size.y));
	Point dstCell(int(dst.x * _cellDims.x / _gameScene->rect().size.x), int(dst.y * _cellDims.y / _gameScene->rect().size.y));
	//printf("src = %s\n", srcCell.str().c_str());
	//printf("dst = %s\n", dstCell.str().c_str());

	for (int i = 0; i < _cellDims.y; i++)
		for (int j = 0; j < _cellDims.x; j++)
			_cells[i][j].reset();

	std::vector<Point> directions = { {-1,0},{1,0},{0,-1},{0,1} };

	// BFS from dst to src with distance from dst computation
	std::queue<Point> q;
	q.push(dstCell);
	_cells[dstCell.y][dstCell.x].distance = 0;
	while (!q.empty()) 
	{
		Point u = q.front();
		//printf("Extracting %s\n", u.str().c_str());
		q.pop();

		for (auto& d : directions) 
		{
			Point v = u + d;
			if (v.x >= 0 && v.y >= 0 && v.x < _cellDims.x && v.y < _cellDims.y &&
				_cells[v.y][v.x].free && !_cells[v.y][v.x].visited)
			{
				//printf("Visiting %s\n", v.str().c_str());
				_cells[v.y][v.x].visited = true;
				//printf("Setting %s distance to %d\n", v.str().c_str(), _cells[u.y][u.x].distance + 1);
				_cells[v.y][v.x].distance = _cells[u.y][u.x].distance + 1;
				q.push({ v.x, v.y });

				if (v == srcCell)
				{
					//printf("arrivato\n");
					//break;
				}
			}
		}
	}

	Point bestMove = { 0,0 };
	int bestDist = std::numeric_limits<int>::max();
	for (auto& d : directions) 
	{
		Point v = srcCell + d;
		printf("_cells[%d][%d].distance = %d\n", v.y, v.x, _cells[v.y][v.x].distance);
		if (v.x >= 0 && v.y >= 0 && v.x < _cellDims.x && v.y < _cellDims.y &&
			_cells[v.y][v.x].free && _cells[v.y][v.x].distance < bestDist) 
		{
			bestDist = _cells[v.y][v.x].distance;
			bestMove = v;
		}
	}

	printf("best move = %s\n", bestMove.str().c_str());
	return _cells[bestMove.y][bestMove.x].center;
}

