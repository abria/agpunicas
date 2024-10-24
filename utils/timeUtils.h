// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2023 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <cstdarg>
#include <vector>
#include <string>
#include <chrono>
#include <iostream>

namespace agp
{
	template <class T>
	class Timer
	{
		private:

			std::chrono::time_point<std::chrono::high_resolution_clock> _t0;

		public:

			Timer() { start(); }

			void start()
			{
				_t0 = std::chrono::high_resolution_clock::now();
			}

			T restart()
			{
				T t = elapsed();
				start();
				return t;
			}

			T elapsed()
			{
				std::chrono::duration<T> elapsed = std::chrono::high_resolution_clock::now()-_t0;
				return elapsed.count();
			}
	};

	class FPS
	{
		private:

			std::chrono::time_point<std::chrono::system_clock> _t0;
			long long _deltaTime;
			unsigned int _framesCount;
			float _lastFPS;

		public:

			FPS()
			{
				_t0 = std::chrono::system_clock::now();
				_framesCount = 0;
				_lastFPS = 0;
				_deltaTime = 0;
			}

			float lastFPS() { return _lastFPS; }

			// returns true if FPS has been updated
			inline bool update(bool print = true)
			{
				_framesCount++;
				_deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _t0).count();
				if (_deltaTime >= 1000.0f)
				{
					_lastFPS = 1000.0f*_framesCount / _deltaTime;
					_t0 = std::chrono::system_clock::now();
					_framesCount = 0;

					if (print)
						printf("FPS = %.0f\n", _lastFPS);

					return true;
				}
				else
					return false;
			}
	};

	class Profiler
	{
		private:

			std::string _name;
			long long _accumulator;
			long long _count;
			int _refresh_ms;
			std::chrono::time_point<std::chrono::high_resolution_clock> _t0;
			std::chrono::time_point<std::chrono::system_clock> _refreshT0;

		public:

			Profiler(const std::string& name, int refresh_ms = 10000)
			{
				_name = name;
				_refresh_ms = refresh_ms;
				_accumulator = 0;
				_count = 0;
				_refreshT0 = std::chrono::system_clock::now();
			}

			inline void begin()
			{
				_t0 = std::chrono::high_resolution_clock::now();
			}

			inline void end()
			{
				_accumulator += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _t0).count();
				_count++;

				long long _deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _refreshT0).count();
				if (_deltaTime >= _refresh_ms)
				{
					printf("Profiler[%s] -> %.0f microseconds\n", _name.c_str(), double(_accumulator)/_count);
					_refreshT0 = std::chrono::system_clock::now();
					_accumulator = 0;
					_count = 0;
				}
			}
	};
}