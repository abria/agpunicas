// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#ifdef WITH_TTF
#include <map>
#include <string>
#include "Singleton.h"
#include "SDL_ttf.h"

namespace agp
{
	class Fonts;
}

// Fonts (singleton)
// - loads all fonts from /fonts once when started
class agp::Fonts : public Singleton<Fonts>
{
	friend class Singleton<Fonts>;

	private:

		std::map< std::string, TTF_Font*> _fonts;

		// constructor accessible only to Singleton (thanks to friend declaration)
		Fonts();

		static constexpr int pointSize = 100;	// font resolution for rasterization

	public:

		~Fonts();

		// getter
		TTF_Font* font(const std::string& name);
};


#endif