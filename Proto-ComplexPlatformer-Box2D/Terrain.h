// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "StaticObject.h"
#include <vector>

namespace agp
{
	class Terrain;
}

// Terrain class.
// - provides base class for walkable surfaces, including slopes
class agp::Terrain : public StaticObject
{
	protected:

		bool _slope;

	public:

		Terrain(GameScene* scene, const LineF& surface, Sprite* sprite = nullptr, int layer = 0);
		virtual ~Terrain() {}

		virtual bool slope() const { return _slope; }
		virtual Vec2Df normal();

		virtual std::string name() override {
			return strprintf("Terrain%s[%d]", (_slope ? "Slope" : ""), _id);
		}

		// helper function to generate terrain sequence
		static void TerrainSequence(
			GameScene* scene,
			const PointF& start,
			const std::vector<PointF>& increments,
			Sprite* sprite = nullptr,
			int layer = 0);
};