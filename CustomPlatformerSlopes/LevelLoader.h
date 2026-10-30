// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <string>

namespace agp
{
	class Scene;
	class LevelData;
	class PlatformerGameScene;
	class LevelLoader;
}

// Stateless level loading utilities.
class agp::LevelLoader
{
	private:

		LevelLoader() = delete;
		static void loadJson(PlatformerGameScene* world, const LevelData& level);

	public:

		static Scene* load(const std::string& name);
};
