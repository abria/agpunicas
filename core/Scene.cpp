// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include "Scene.h"
#include "Object.h"
#include "View.h"
#include "timeUtils.h"

using namespace agp;

Scene::Scene(const RectF& rect, const Point& pixelUnitSize)
{
	_rect = rect;
	_pixelUnitSize = pixelUnitSize;
	_backgroundColor = Color(0, 0, 0, 0);
	_active = true;
	_visible = true;
	_blocking = false;
	_view = nullptr;
	_rectsVisible = false;
}

Scene::~Scene()
{
	for(auto& obj : _objects)
		delete obj;
}

void Scene::newObject(Object* obj)
{
	_newObjects.insert(obj);
}

void Scene::killObject(Object* obj)
{
	_deadObjects.insert(obj);
	obj->_killed = true;
}

void Scene::refreshObjects()
{
	for (auto& obj : _newObjects)
		_objects.emplace_back(obj);
	_newObjects.clear();

	for (auto it = _deadObjects.begin(); it != _deadObjects.end(); )
	{
		Object* obj = *it;

		if (obj->_itersFromKilled < 2)
		{
			++it;
			continue;
		}

		auto jt = std::find(_objects.begin(), _objects.end(), obj);
		if (jt != _objects.end()) 
		{
			size_t idxToRemove = std::distance(_objects.begin(), jt);
			_objects[idxToRemove] = _objects.back();
			_objects.pop_back();
		}

		// Erase the element from the set and advance the iterator safely
		it = _deadObjects.erase(it); // 'erase' returns an iterator to the next element
		delete obj;
	}
}

std::vector<Object*> Scene::objects()
{
	return _objects;
}

Objects Scene::objects(const RectF& cullingRect)
{
	Objects objectsInRect;
	for (auto& obj : _objects)
		if (obj->intersectsRectShallow(cullingRect))
			objectsInRect.push_back(obj);

	return objectsInRect;
}

Objects Scene::objects(const PointF& containPoint)
{
	Objects objectsSelected;
	for (auto& obj : _objects)
		if (obj->contains(containPoint))
			objectsSelected.push_back(obj);

	return objectsSelected;
}

Objects Scene::raycast(const LineF& line, std::list<float>* hitTimes)
{
	std::vector<std::pair<Object*, float>> hits;

	auto objectsInRect = objects(line.boundingRect(_rect.yUp));
	for (Object* obj : objectsInRect)
	{
		float tNear;
		if (obj->intersectsLine(line, tNear))
			hits.push_back({ obj, tNear });
	}

	// sort the hits based on the distance along the line (tNear):
	std::sort(hits.begin(), hits.end(),
		[](const std::pair<Object*, float>& a, const std::pair<Object*, float>& b) {
			return a.second < b.second;
		});

	// extract the Object pointers from the sorted hits:
	Objects result;
	for (const auto& hit : hits)
	{
		result.push_back(hit.first);
		if (hitTimes)
			(*hitTimes).push_back(hit.second);
	}

	return result;
}

Object* Scene::raycastNearest(const LineF& line, float& tNear)
{
	std::list<float> hitTimes;
	auto objects = raycast(line, &hitTimes);
	if (objects.size())
	{
		tNear = hitTimes.front();
		return objects.front();
	}
	else 
		return nullptr;
}

bool Scene::isEmpty(const RectF& rect)
{
	for (auto& obj : _objects)
		if (obj->intersectsRect(rect))
			return false;

	return true;
}

void Scene::render()
{
	if (_visible && _view)
		_view->render();
}

void Scene::update(float timeToSimulate)
{
	refreshObjects();

	auto iter = _schedulers.begin();
	for (; iter != _schedulers.end(); )
	{
		if (iter->second.on())
		{
			iter->second.update(timeToSimulate);
			++iter;
		}
		else
			iter = _schedulers.erase(iter);
	}
}

void Scene::schedule(const std::string& id, float delaySeconds, std::function<void()> action, int loop, bool overwrite)
{
	if (overwrite || _schedulers.find(id) == _schedulers.end())
		_schedulers[id] = Scheduler(delaySeconds, action, loop);
}

void Scene::event(SDL_Event& evt)
{
	if (evt.type == SDL_WINDOWEVENT && _view)
		_view->updateViewport();
}

void Scene::objectMoved(Object* obj)
{
	// nothing to do here
}