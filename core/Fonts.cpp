// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#ifdef WITH_TTF
#include "Fonts.h"
#include "SDL.h"
#include "fileUtils.h"
#include <iostream>

using namespace agp;

Fonts::Fonts()
{
	if (TTF_Init())
		throw SDL_GetError();

	auto fontFiles = getFilesInDirectory(std::string(SDL_GetBasePath()) + "fonts");
	for (auto& f : fontFiles)
	{
		std::string name = getFileName(f, false);

		TTF_Font* font = TTF_OpenFont((std::string(SDL_GetBasePath()) + "fonts/" + f).c_str(), pointSize);
		if (!font)
			std::cerr << TTF_GetError() << "\n";
		else
			_fonts[name] = font;
	}
}

Fonts::~Fonts()
{
	for (auto& entry : _fonts)
		TTF_CloseFont(entry.second);

	TTF_Quit();
}

TTF_Font* Fonts::font(const std::string& name)
{
	if (_fonts.find(name) == _fonts.end())
	{
		std::cerr << "Cannot find font \"" << name << "\"\n";
		return nullptr;
	}

	return _fonts[name];
}


#endif