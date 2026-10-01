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

	// Measures the frequency of calls to update(), in Hz.
	class FrequencyMeter
	{
		private:

			std::chrono::time_point<std::chrono::system_clock> _t0;
			long long _deltaTime;
			unsigned int _callsCount;
			float _lastFrequency;

		public:

			FrequencyMeter()
			{
				_t0 = std::chrono::system_clock::now();
				_callsCount = 0;
				_lastFrequency = 0;
				_deltaTime = 0;
			}

			float lastFrequency() { return _lastFrequency; }

			// counts one call and returns true if the frequency has been updated
			inline bool update(bool print = true)
			{
				_callsCount++;
				_deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _t0).count();
				if (_deltaTime >= 1000.0f)
				{
					_lastFrequency = 1000.0f*_callsCount / _deltaTime;
					_t0 = std::chrono::system_clock::now();
					_callsCount = 0;

					if (print)
						printf("Frequency = %.0f Hz\n", _lastFrequency);

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