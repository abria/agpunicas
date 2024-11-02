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
}

// Scene class to be used in a Scene stack
// - provides base class for more specific scenes (e.g. GameScene, UIScene)
//   with interface methods like rendering, logic update, and event processing
// - contains objects sorted by ascending z-level (painter algorithm)
// - provides efficient access to objects
// - provides globale action scheduling
class agp::Scene
{
	public:

		typedef std::map< int, std::vector< Object*> > ObjectsMap;
		typedef std::list< Object*> ObjectsList;
		typedef std::set< Object*> ObjectsSet;
		typedef std::list< std::pair<int, Object*>> ObjectsLayersList;

	protected:
		
		ObjectsMap  _sortedObjects;	// objects sorted by ascending z-level
		ObjectsSet _newObjects;		// new objects that need to be added
		ObjectsSet _deadObjects;	// dead objects that need to be deallocated
		ObjectsLayersList _changeLayerObjects;	// objects that need to change layer
		RectF _rect;				// the scene (world) rectangle
		Point _pixelUnitSize;		// unit size in pixels
		Color _backgroundColor;		// background color
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
		void setBackgroundColor(const Color& c) { _backgroundColor = c; }
		bool visible() { return _visible; }
		void setVisible(bool on) { _visible = on; }
		bool active() { return _active; }
		void setActive(bool on) { _active = on; }
		bool blocking() { return _blocking; }
		void setBlocking(bool on) { _blocking = on; }
		bool rectsVisible() const { return _rectsVisible; }
		void toggleRects() { _rectsVisible = !_rectsVisible; }
		Point pixelUnitSize() { return _pixelUnitSize; }

		// add/remove objects
		void newObject(Object* obj);
		void killObject(Object* obj);
		void changeLayerObject(Object* obj, int newLayer);
		void refreshObjects();

		// objects selection methods
		virtual ObjectsList objects();
		virtual ObjectsList objects(const RectF& cullingRect);
		virtual ObjectsList objects(const PointF& containPoint);

		// render
		virtual void render();

		// update
		virtual void update(float timeToSimulate);

		// scheduling
		virtual void schedule(const std::string& id, float delaySeconds, std::function<void()> action, int loop = 0, bool overwrite = true);

		// event handler
		virtual void event(SDL_Event& evt);
};