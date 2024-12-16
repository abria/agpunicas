// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <map>
#include <vector>
#include <list>
#include <set>
#include "geometryUtils.h"
#include "graphicsUtils.h"
#include "Scheduler.h"

namespace agp
{
	class Object;
	class Scene;
	class View;
	class RenderableObject;
}

// Scene class to be used in a Scene stack
// - provides base class for more specific scenes (e.g. GameScene, UIScene)
//   with interface methods like rendering, logic update, and event processing
// - contains objects sorted by ascending z-level (painter algorithm)
// - provides efficient access to objects
// - provides global action scheduling
class agp::Scene
{
	public:

		typedef std::map< int, std::vector< Object*> > ObjectsMap;
		typedef std::list< Object*> ObjectsList;
		typedef std::set< Object*> ObjectsSet;
		typedef std::list< std::pair<int, Object*>> ObjectsLayersList;
		typedef std::vector < RenderableObject*> Renderables;

	protected:
		
		ObjectsMap _sortedObjects;	// objects sorted by ascending z-level
		ObjectsSet _newObjects;		// new objects that need to be added
		ObjectsSet _deadObjects;	// dead objects that need to be deallocated
		ObjectsLayersList _changeLayerObjects;	// objects that need to change layer
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
		const RectF& rect() { return _rect; }
		void setRect(const RectF& r) { _rect = r; }
		View* view() { return _view; }
		const Color& backgroundColor() { return _backgroundColor; }
		virtual void setBackgroundColor(const Color& c) { _backgroundColor = c; }
		Renderables backgroundImages() { return _backgroundImages; }
		virtual void addBackgroundImage(RenderableObject* img) { _backgroundImages.push_back(img); }
		bool visible() { return _visible; }
		virtual void setVisible(bool on) { _visible = on; }
		bool active() { return _active; }
		virtual void setActive(bool on) { _active = on; }
		bool blocking() { return _blocking; }
		virtual void setBlocking(bool on) { _blocking = on; }
		bool rectsVisible() const { return _rectsVisible; }
		virtual void toggleRects() { _rectsVisible = !_rectsVisible; }
		Point pixelUnitSize() { return _pixelUnitSize; }

		// add/remove objects
		void newObject(Object* obj);
		void killObject(Object* obj);
		void changeLayerObject(Object* obj, int newLayer);
		void refreshObjects();

		// geometric queries
		virtual ObjectsList objects();
		virtual ObjectsList objects(const RectF& cullingRect);
		virtual ObjectsList objects(const PointF& containPoint);
		virtual ObjectsList raycast(const LineF& line, std::list<float> *hitTimes = nullptr);
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
};