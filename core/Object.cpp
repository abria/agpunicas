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
	_itersFromKilled = 0;
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

	if (_killed)
		_itersFromKilled++;

	auto iter = _schedulers.begin();
	for (; iter != _schedulers.end(); )
	{
		if (iter->second.on())
		{
			iter->second.update(dt);
			++iter;
		}
		else
			iter = _schedulers.erase(iter);
	}
}

void Object::schedule(const std::string& id, float delaySeconds, std::function<void()> action, int loop, bool overwrite)
{
	if (overwrite || _schedulers.find(id) == _schedulers.end())
		_schedulers[id] = Scheduler(delaySeconds, action, loop);
}

void Object::unschedule(const std::string& id)
{
	if (_schedulers.find(id) != _schedulers.end())
		_schedulers.erase(id);
}

void Object::kill()
{
	_scene->killObject(this);
}
