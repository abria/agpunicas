// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <string>

namespace agp
{
	namespace ComplexPlatformer
	{
		// Version components
		constexpr int VERSION_MAJOR = 1;
		constexpr int VERSION_MINOR = 3;
		constexpr int VERSION_PATCH = 3;

		// Function to retrieve the version string
		inline static std::string VERSION() {
			return std::to_string(VERSION_MAJOR) + "." +
				   std::to_string(VERSION_MINOR) + "." +
				   std::to_string(VERSION_PATCH);
		}
	}
}
