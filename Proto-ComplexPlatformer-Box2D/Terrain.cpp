// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Terrain.h"
#include "Scene.h"

using namespace agp;

Terrain::Terrain(GameScene* scene, const LineF& surface, Sprite* sprite, int layer) :
	StaticObject(scene, RotatedRectF(surface, 0.5f, true), sprite, layer)
{
	_slope = _obb.angle != 0;
}

Vec2Df Terrain::normal()
{
	return Vec2Df(std::cosf(_obb.angle), std::sinf(_obb.angle)).perp(true).norm();
}

void Terrain::TerrainSequence(
	GameScene* scene,
	const PointF& start,
	const std::vector<PointF>& increments,
	Sprite* sprite,
	int layer)
{
	std::vector<PointF> points = { start };
	for (int k = 0; k < increments.size(); k++)
	{
		points.push_back(points.back() + increments[k]);
		new Terrain(scene, LineF(points[k], points[k + 1]), sprite, layer);
	}
}