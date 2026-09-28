// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it).
// All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "json.hpp"
#include "geometryUtils.h"

namespace agp
{
	class GameScene;
	class LevelData;
}

// LevelData class
// - reads and validates level editor json files
// - shares geometry conversion between games and editor
// - game-specific object creation stays in each LevelLoader
class agp::LevelData
{
	protected:

		std::string _path;
		nlohmann::ordered_json _json;

		static void validate(const nlohmann::ordered_json& json);

	public:

		LevelData(const std::string& path);

		const nlohmann::ordered_json& json() const { return _json; }
		const nlohmann::ordered_json& objects() const { return _json.at("objects"); }
		std::string category(const nlohmann::ordered_json& object) const;

		// scene settings (optional in legacy editor files)
		RectF sceneRect() const;
		Point pixelUnitSize() const;
		float timeStep() const;
		void configure(GameScene* scene) const;

		// geometry (json angles are in degrees, runtime angles in radians)
		static RectF rect(const nlohmann::ordered_json& json);
		static RotatedRectF rotRect(const nlohmann::ordered_json& object);
		static std::vector<PointF> multiline(const nlohmann::ordered_json& json);

		static void save(const std::string& path, const nlohmann::ordered_json& json);
};
