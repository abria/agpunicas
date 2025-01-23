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
#include "Scheduler.h"
#include "stringUtils.h"
#include "geometryUtils.h"

namespace agp
{
	class Scene;
	class Object;
}

// Object (or game object, or entity, or actor) abstract class.
// Suitable for monolithic class hierarchies in simple 2D games.
// - auto-adds itself to the scene
// - stores object rect (position and size)
// - stores schedulers for action scripting
// - stores object layer in the scene (useful for sorting e.g. for Painter's algorithm)
// - stores general state flags
// - offers update and schedule methods, and simple geometric queries
class agp::Object
{
	private:

		RectF _rect;	// subclasses must use getter/setter

	protected:

		Scene* _scene;
		int _layer;
		int _id;
		bool _freezed;	// if false, does not update
		bool _killed;
		int _itersFromKilled;
		std::map<std::string, Scheduler> _schedulers;

		friend class Scene;

	public:

		Object(Scene* scene, const RectF& rect, int layer = 0);
		virtual ~Object() {}

		// getters/setters
		const RectF& rect() const { return _rect; }
		virtual void setRect(const RectF& newRect);
		virtual const PointF& pos() const { return _rect.pos; }
		virtual void setPos(const PointF& newPos);
		PointF size() const { return _rect.size; }
		virtual void setSize(const PointF& newSize) { _rect.size = newSize; }
		int layer() const { return _layer; }
		virtual void setLayer(int newLayer) { _layer = newLayer; }
		bool freezed() const { return _freezed; }
		virtual void setFreezed(bool on) { _freezed = on; }
		void toggleFreezed() { _freezed = !_freezed; }
		Scene* scene() const { return _scene; }

		// geometric queries
		virtual bool contains(const Vec2Df& p) { return _rect.contains(p); }
		virtual bool intersectsRect(const RectF& r) { return _rect.intersects(r); }
		virtual bool intersectsRectShallow(const RectF& r) { return _rect.intersects(r); }
		virtual bool intersectsLine(const LineF& line, float& tNear) { float tFar;  return _rect.intersectsLine(line.start, line.end, tNear, tFar); }

		// core game logic (physics, ...)
		virtual void update(float dt);

		// scheduling
		virtual void schedule(const std::string& id, float delaySeconds, std::function<void()> action, int loop = 0, bool overwrite = true);
		virtual void unschedule(const std::string& id);

		// type conversion
		template <class T>
		T to() { return dynamic_cast<T>(this); }

		// kill
		virtual void kill();

		// debugging
		int id() const { return _id; }
		virtual std::string name() { return strprintf("Object[%d]", _id); }
};