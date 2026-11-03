// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Object.h"
#include "Scene.h"
#include <cmath>
#include <limits>
#include <stdexcept>

using namespace agp;

static int created = 0;

Object::Object(Scene* scene, const RectF& rect, int layer)
{
	_scene = scene;
	_rect = rect;
	_layer = layer;
	_id = created++;
	_freezed = false;
	_killed = false;
	_scene->newObject(this);
	_timeElapsed = 0;
}

void Object::setRect(const RectF& newRect) 
{ 
	if (newRect != _rect)
	{
		_rect = newRect;

		_scene->objectMoved(this);
	}
}

void Object::setPos(const PointF& newPos) 
{ 
	if (newPos != _rect.pos)
	{
		_rect.pos = newPos;

		_scene->objectMoved(this);
	}
}

void Object::update(float dt)
{
	_timeElapsed += dt;

	// Execute a copy: callbacks may cancel or replace their own scheduler.
	// New tasks start next update; cancelled/replaced tasks in this snapshot are skipped.
	auto pending = _schedulers;
	for (auto& entry : pending)
	{
		if (_killed)
			break;
		auto current = _schedulers.find(entry.first);
		if (current == _schedulers.end() || current->second.revision != entry.second.revision)
			continue;
		entry.second.scheduler.update(dt);
		current = _schedulers.find(entry.first);
		if (current != _schedulers.end() && current->second.revision == entry.second.revision)
		{
			if (entry.second.scheduler.on())
				current->second.scheduler = entry.second.scheduler;
			else
				_schedulers.erase(current);
		}
	}
}

void Object::schedule(const std::string& id, float delaySeconds, std::function<void()> action, int loop, bool overwrite)
{
	if (overwrite || _schedulers.find(id) == _schedulers.end())
		_schedulers[id] = { Scheduler(delaySeconds, action, loop), ++_schedulerRevision };
}

void Object::unschedule(const std::string& id)
{
	if (_schedulers.find(id) != _schedulers.end())
		_schedulers.erase(id);
}

void Object::sequence(const std::string& id,
	const std::vector<std::pair<float, std::function<void()>>>& steps)
{
	// Validate before replacing an existing sequence.
	float delay = 0;
	for (const auto& step : steps)
	{
		delay += step.first;
		if (!std::isfinite(step.first) || step.first < 0 || !std::isfinite(delay) || !step.second)
			throw std::invalid_argument("sequence requires finite non-negative delays and callable actions");
	}

	unscheduleSequence(id);
	delay = 0;
	for (size_t i = 0; i < steps.size(); ++i)
	{
		delay += steps[i].first;
		// Fixed-width indices preserve step order even when several timers expire together.
		std::string index = std::to_string(i);
		index.insert(0, std::numeric_limits<size_t>::digits10 + 1 - index.size(), '0');
		schedule(id + "/" + index, delay, steps[i].second);
	}
}

void Object::unscheduleSequence(const std::string& id)
{
	const std::string prefix = id + "/";
	auto it = _schedulers.lower_bound(prefix);
	while (it != _schedulers.end() && it->first.compare(0, prefix.size(), prefix) == 0)
		it = _schedulers.erase(it);
}

void Object::kill()
{
	_scene->killObject(this);
}
