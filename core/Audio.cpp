// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Audio.h"
#include <SDL3/SDL.h>
#include "fileUtils.h"
#include <algorithm>
#include <iostream>

using namespace agp;

Audio::Audio()
{
	_mixer = nullptr;
	_musicTrack = nullptr;

	if (!SDL_Init(SDL_INIT_AUDIO))
		throw SDL_GetError();

	if (!MIX_Init())
		throw SDL_GetError();

	_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!_mixer)
		throw SDL_GetError();

	_musicTrack = MIX_CreateTrack(_mixer);
	if (!_musicTrack)
		throw SDL_GetError();

	auto soundFiles = getFilesInDirectory(std::string(SDL_GetBasePath()) + "assets/sounds", "*.wav");
	for (auto& f : soundFiles)
	{
		std::string name = getFileName(f, false);
		//printf("sound: \"%s\"\n", name.c_str());

		MIX_Audio* audio = MIX_LoadAudio(_mixer, (std::string(SDL_GetBasePath()) + "assets/sounds/" + f).c_str(), true);
		if (!audio)
			std::cerr << SDL_GetError() << "\n";
		else
			_sounds[name] = audio;
	}

	auto musicFiles = getFilesInDirectory(std::string(SDL_GetBasePath()) + "assets/musics", "*.wav");
	for (auto& f : musicFiles)
	{
		std::string name = getFileName(f, false);
		//printf("music: \"%s\"\n", name.c_str());

		MIX_Audio* audio = MIX_LoadAudio(_mixer, (std::string(SDL_GetBasePath()) + "assets/musics/" + f).c_str(), false);
		if (!audio)
			std::cerr << SDL_GetError() << "\n";
		else
			_musics[name] = audio;
	}

    _soundGains["Mario Jump"] = 1.0f;
    _soundGains["Skid"] = 96.0f / 128.0f;

	//regolo il volume del thwomp
	_soundGains["Thwomp"] = 96.0f / 128.0f;
}

Audio::~Audio()
{
	for (auto& entry : _soundTracks)
		for (auto& track : entry.second)
			MIX_DestroyTrack(track);
	MIX_DestroyTrack(_musicTrack);

	for (auto& entry : _musics)
		MIX_DestroyAudio(entry.second);
	for (auto& entry : _sounds)
		MIX_DestroyAudio(entry.second);

	MIX_DestroyMixer(_mixer);
	MIX_Quit();
}

void Audio::playSound(const std::string& id, int loops, bool forceReplay)
{
	auto sound = _sounds.find(id);
	if (sound == _sounds.end())
		return;

	if (forceReplay)
		stopSound(id);

	auto& tracks = _soundTracks[id];
	tracks.erase(std::remove_if(tracks.begin(), tracks.end(), [](MIX_Track* track)
		{
			if (!MIX_TrackPlaying(track))
			{
				MIX_DestroyTrack(track);
				return true;
			}
			return false;
		}), tracks.end());

	MIX_Track* track = MIX_CreateTrack(_mixer);
	if (!track)
		return;

	MIX_SetTrackAudio(track, sound->second);
	MIX_SetTrackGain(track, _soundGains.count(id) ? _soundGains[id] : 1.0f);

	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
	if (MIX_PlayTrack(track, props))
		tracks.push_back(track);
	else
		MIX_DestroyTrack(track);
	SDL_DestroyProperties(props);
}

void Audio::stopSound(const std::string& id)
{
	auto it = _soundTracks.find(id);
	if (it == _soundTracks.end())
		return;

	for (auto& track : it->second)
	{
		MIX_StopTrack(track, 0);
		MIX_DestroyTrack(track);
	}
	it->second.clear();
}

void Audio::pauseAllLoopingSounds()
{
	for (auto& entry : _soundTracks)
		for (auto& track : entry.second)
			if (MIX_TrackPlaying(track) && MIX_GetTrackLoops(track) != 0)
				MIX_PauseTrack(track);
}

void Audio::resumeAllLoopingSounds()
{
	for (auto& entry : _soundTracks)
		for (auto& track : entry.second)
			if (MIX_TrackPaused(track) && MIX_GetTrackLoops(track) != 0)
				MIX_ResumeTrack(track);
}

void Audio::playMusic(const std::string& id, int loops)
{
	if (_musics.find(id) == _musics.end())
	{
		std::cerr << "Cannot find music \"" << id << "\"\n";
		return;
	}

	MIX_SetTrackAudio(_musicTrack, _musics[id]);
	SDL_PropertiesID props = SDL_CreateProperties();
	SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
	MIX_PlayTrack(_musicTrack, props);
	SDL_DestroyProperties(props);
}

void Audio::resumeMusic()
{
	MIX_ResumeTrack(_musicTrack);
}

void Audio::pauseMusic()
{
	MIX_PauseTrack(_musicTrack);
}

void Audio::haltMusic()
{
	MIX_StopTrack(_musicTrack, 0);
}
