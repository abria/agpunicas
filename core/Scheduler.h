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
		bool _active;
		int _loop;		// -1 = infinite loop

	public:

		Scheduler()
		{
			_task = []() {};
			_reverseTimer = 0;
			_delaySeconds = 0;
			_loop = 0;
			_active = false;
		}

		Scheduler(float delaySeconds, std::function<void()> task, int loop = 0)
		{
			_task = task;
			_delaySeconds = delaySeconds;
			_reverseTimer = delaySeconds;
			_loop = loop;
			_active = delaySeconds >= 0;
		}

		void update(float dt)
		{
			if (on())
			{
				_reverseTimer -= dt;
				if (_reverseTimer <= 0)
				{
					if (_loop != 0)
					{
						_reverseTimer = _delaySeconds;
						if (_loop > 0)
							_loop--;
					}
					else
						_active = false;
					// A callback may replace this scheduler. Finish state changes first.
					std::function<void()> task;
					task.swap(_task);
					task();
					if (!_task)
						_task.swap(task);
				}
			}
		}

		bool on() const { return _active; }
};