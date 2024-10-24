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
	class GameScene;
	class LevelLoader;
}

// LevelLoader (singleton)
// - provides game scene creation methods
class agp::LevelLoader
{
	protected:

		// constructor inaccesible due to singleton
		LevelLoader();

	public:

		// singleton
		static LevelLoader* instance();

		GameScene* load(const std::string& name);
};
