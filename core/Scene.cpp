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
	for (auto& layer : _sortedObjects)
		for(auto& obj : layer.second)
			delete obj;
}

void Scene::newObject(Object* obj)
{
	_newObjects.insert(obj);
}

void Scene::killObject(Object* obj)
{
	_deadObjects.insert(obj);
}

void Scene::changeLayerObject(Object* obj, int newLayer)
{
	_changeLayerObjects.push_back(std::pair<int, Object*>(newLayer, obj));
}

void Scene::refreshObjects()
{
	for(auto& obj : _newObjects)
		_sortedObjects[obj->layer()].emplace_back(obj);
	_newObjects.clear();

	for (auto& p : _changeLayerObjects)
	{
		auto& layer = _sortedObjects[p.second->layer()];
		auto removeIt = std::remove(layer.begin(), layer.end(), p.second);
		if (removeIt == layer.end())
			std::cerr << "Cannot remove " << p.second->name() << " from layer " << p.second->layer() << " when changing layer: object not found\n";
		layer.erase(removeIt, layer.end());

		if (std::find(_sortedObjects[p.first].begin(), _sortedObjects[p.first].end(), p.second) == _sortedObjects[p.first].end())
		{
			_sortedObjects[p.first].emplace_back(p.second);
			p.second->_layer = p.first;
		}
		else
			std::cerr << "Cannot re-insert " << p.second->name() << " in layer " << p.first << " when changing layer: object already present\n";

	}
	_changeLayerObjects.clear();

	for (auto& obj : _deadObjects)
	{
		auto& layer = _sortedObjects[obj->layer()];
		auto removeIt = std::remove(layer.begin(), layer.end(), obj);
		if (removeIt == layer.end())
			std::cerr << "Cannot remove " << obj->name() << " from layer " << obj->layer() << ": object not found\n";
		layer.erase(removeIt, layer.end());
		delete obj;
	}
	_deadObjects.clear();
}

std::list<Object*> Scene::objects()
{
	std::list<Object*> allObjects;
	for (auto& layer : _sortedObjects)
		for (auto& obj : layer.second)
			allObjects.push_back(obj);

	return allObjects;
}

std::list<Object*> Scene::objects(const RectF& cullingRect)
{
	std::list<Object*> objectsInRect;
	for (auto& layer : _sortedObjects)
		for (auto& obj : layer.second)
			if (cullingRect.intersects(obj->rect()))
				objectsInRect.push_back(obj);

	return objectsInRect;
}

std::list<Object*> Scene::objects(const PointF& containPoint)
{
	std::list<Object*> objectsSelected;
	for (auto& layer : _sortedObjects)
		for (auto& obj : layer.second)
			if (obj->contains(containPoint))
				objectsSelected.push_back(obj);

	return objectsSelected;
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