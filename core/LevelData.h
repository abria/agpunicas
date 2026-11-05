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
	class LevelData;
}

// game source folder, defined in the game's CMakeLists.txt
// (empty: levels are read and saved only next to the executable)
#ifndef AGP_SOURCE_DIR
#define AGP_SOURCE_DIR ""
#endif

// LevelData class
// - reads and validates level editor json files
// - shares geometry conversion between games and editor
// - game-specific object creation stays in each LevelLoader
class agp::LevelData
{
	protected:

		nlohmann::ordered_json _json;

		static void validate(const nlohmann::ordered_json& json);

	public:

		LevelData(const std::string& path);

		const nlohmann::ordered_json& json() const { return _json; }
		const nlohmann::ordered_json& objects() const { return _json.at("objects"); }
		std::string category(const nlohmann::ordered_json& object) const;

		// geometry (json angles are in degrees, runtime angles in radians)
		static RectF rect(const nlohmann::ordered_json& json);
		// Axis-aligned geometry in the requested coordinate system.
		static RectF axisAlignedRect(const nlohmann::ordered_json& object, bool yUp = false);
		static RotatedRectF rotRect(const nlohmann::ordered_json& object);
		static std::vector<PointF> multiline(const nlohmann::ordered_json& json);

		static void save(const std::string& path, const nlohmann::ordered_json& json);

		// level file shared by game and editor; relativePath is the same in the source
		// folder and next to the executable, where CMake copies it at every build
		// - source copy when reachable: editor saves survive the next build
		// - otherwise the copy next to the executable (e.g. game moved elsewhere)
		static std::string path(const std::string& relativePath, const std::string& sourceDir);
};
