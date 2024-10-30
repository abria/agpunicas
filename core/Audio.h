// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "SDL_mixer.h"
#include <map>
#include <string>
#include "Singleton.h"

namespace agp
{
	class Audio;
}

// Audio (singleton)
// - loads all sounds and musics once when started
// - offers methods to play/control sounds and musics indexed by id
class agp::Audio : public Singleton<Audio>
{
	friend class Singleton<Audio>;

	private:

		std::map< std::string, Mix_Chunk*> _sounds;
		std::map< std::string, Mix_Music*> _musics;

		// constructor accessible only to Singleton (thanks to friend declaration)
		Audio();

	public:

		~Audio();

		// controls
		void playSound(const std::string & id, int loops = 0);
		void playMusic(const std::string& id, int loops = -1);
		void resumeMusic();
		void pauseMusic();
		void haltMusic();
};