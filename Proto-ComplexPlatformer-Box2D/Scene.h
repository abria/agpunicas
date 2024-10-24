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
#include "geometryUtils.h"
#include "graphicsUtils.h"

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
class agp::Scene
{
	public:

		typedef std::map< int, std::vector< Object*> > ObjectsMap;
		typedef std::list< Object*> ObjectsList;

	protected:

		ObjectsMap _sortedObjects;	// objects sorted by ascending z-level
		ObjectsList _newObjects;	// new objects that need to be added
		ObjectsList _deadObjects;	// dead objects that need to be dealloacted
		RectF _rect;				// the scene (world) rectangle
		Color _backgroundColor;		// background color
		View* _view;				// associated view for rendering
		bool _visible;				// whether has to be rendered
		bool _active;				// whether has to be updated
		bool _blocking;				// whether blocks events propagation and logic update
									// for scenes in lower layers of the stack

	public:

		Scene(const RectF& r);
		virtual ~Scene();

		// getters/setters
		const RectF& rect() { return _rect; }
		void setRect(const RectF& r) { _rect = r; }
		const Color& backgroundColor() { return _backgroundColor; }
		void setBackgroundColor(const Color& c) { _backgroundColor = c; }
		bool visible() { return _visible; }
		void setVisible(bool on) { _visible = on; }
		bool active() { return _active; }
		void setActive(bool on) { _active = on; }
		bool blocking() { return _blocking; }
		void setBlocking(bool on) { _blocking = on; }
		View* view() { return _view; }

		// add/remove objects
		void newObject(Object* obj);
		void killObject(Object* obj);
		void refreshObjects();

		// objects selection methods
		virtual ObjectsList objects();
		virtual ObjectsList objects(const RectF& cullingRect);
		virtual ObjectsList objects(const PointF& containPoint);

		// render
		virtual void render();

		// update
		virtual void update(float timeToSimulate);

		// event handler
		virtual void event(SDL_Event& evt);
};