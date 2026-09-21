// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <vector>
#include <set>
#include <list>
#include <map>
#include "geometryUtils.h"
#include "graphicsUtils.h"
#include "Scheduler.h"

namespace agp
{
	class Object;
	class Scene;
	class View;
	class RenderableObject;

	typedef std::vector< Object*> Objects;
	typedef std::set< Object*> ObjectsSet;
	typedef std::vector < RenderableObject*> Renderables;
}

// Scene class to be used in a Scene stack
// - provides base class for more specific scenes (e.g. GameScene, UIScene)
//   with interface methods like rendering, logic update, and event processing
// - provides simple container (std::vector) for scene objects with deferred add/remove
// - provides global action scheduling
class agp::Scene
{
	protected:
		
		Objects _objects;
		ObjectsSet _newObjects;		// new objects that need to be added
		ObjectsSet _deadObjects;	// dead objects that need to be deallocated
		RectF _rect;				// the scene (world) rectangle
		Point _pixelUnitSize;		// unit size in pixels
		Color _backgroundColor;		// background color
		Renderables _backgroundImages; // background images
		View* _view;				// associated view for rendering
		bool _visible;				// whether has to be rendered
		bool _active;				// whether has to be updated
		bool _blocking;				// whether blocks events propagation and logic update
									// for scenes in lower layers of the stack
		bool _rectsVisible;			// whether objects rects are visible
		std::map<std::string, Scheduler> _schedulers;

	public:

		Scene(const RectF& rect, const Point& pixelUnitSize);
		virtual ~Scene();

		// getters/setters
		const RectF& rect() const { return _rect; }
		void setRect(const RectF& r) { _rect = r; }
		View* view() const { return _view; }
		const Color& backgroundColor() { return _backgroundColor; }
		virtual void setBackgroundColor(const Color& c) { _backgroundColor = c; }
		Renderables backgroundImages() const { return _backgroundImages; }
		virtual void addBackgroundImage(RenderableObject* img) { _backgroundImages.push_back(img); }
		bool visible() const { return _visible; }
		virtual void setVisible(bool on) { _visible = on; }
		bool active() const { return _active; }
		virtual void setActive(bool on) { _active = on; }
		bool blocking() const { return _blocking; }
		virtual void setBlocking(bool on) { _blocking = on; }
		bool rectsVisible() const { return _rectsVisible; }
		virtual void toggleRects() { _rectsVisible = !_rectsVisible; }
		Point pixelUnitSize() const { return _pixelUnitSize; }

		// add/remove objects
		virtual void newObject(Object* obj);
		virtual void killObject(Object* obj);
		virtual void refreshObjects();

		// geometric queries
		virtual Objects objects();
		virtual Objects objects(const RectF& cullingRect);
		virtual Objects objects(const PointF& containPoint);
		virtual Objects raycast(const LineF& line, std::list<float> *hitTimes = nullptr);
		virtual Object* raycastNearest(const LineF& line, float& tNear);
		virtual bool isEmpty(const RectF& rect);

		// render
		virtual void render();

		// update
		virtual void update(float timeToSimulate);

		// scheduling
		virtual void schedule(const std::string& id, float delaySeconds, std::function<void()> action, int loop = 0, bool overwrite = true);

		// event handler
		virtual void event(SDL_Event& evt);

		// scene events
		virtual void objectMoved(Object* obj);
};