// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <functional>

namespace agp
{
	class Scheduler;
}

// Scheduler class
// -  performs a given task with the given delay
class agp::Scheduler
{
	private:

		std::function<void()> _task;
		float _delaySeconds;
		float _reverseTimer;
		int _loop;		// -1 = infinite loop

	public:

		Scheduler()
		{
			_task = []() {};
			_reverseTimer = 0;
			_delaySeconds = 0;
			_loop = 0;
		}

		Scheduler(float delaySeconds, std::function<void()> task, int loop = 0)
		{
			_task = task;
			_delaySeconds = delaySeconds;
			_reverseTimer = delaySeconds;
			_loop = loop;
		}

		void update(float dt)
		{
			if (on())
			{
				_reverseTimer -= dt;
				if (_reverseTimer <= 0)
				{
					_task();
					if (_loop != 0)
					{
						_reverseTimer = _delaySeconds;
						_loop--;
					}
				}
			}
		}

		bool on() { return _reverseTimer > 0; }
};