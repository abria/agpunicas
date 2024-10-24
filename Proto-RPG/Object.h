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
// Suitable for monolithic class hierarchies in simple platformer games.
// - auto-adds itself to the scene
// - stores object rect (position and size)
// - stores schedulers for action scripting
// - stores object layer in the scene (useful for sorting e.g. for Painter's algorithm)
// - stores general state flags
// - offers update and schedule methods
class agp::Object
{
	protected:

		Scene* _scene;
		RectF _rect;
		int _layer;
		int _id;
		bool _freezed;	// if false, does not update
		std::map<std::string, Scheduler> _schedulers;

	public:

		Object(Scene* scene, const RectF& rect, int layer = 0);
		virtual ~Object() {}

		// getters/setters
		const RectF& rect() const { return _rect; }
		void setRect(const RectF& rect) { _rect = rect; }
		int layer() { return _layer; }
		bool freezed() { return _freezed; }
		void setFreezed(bool on) { _freezed = on; }
		void toggleFreezed() { _freezed = !_freezed; }
		bool contains(const Vec2Df& p) { return _rect.contains(p); }

		// core game logic (physics, ...)
		virtual void update(float dt);

		// scheduling
		virtual void schedule(const std::string& id, float delaySeconds, std::function<void()> action, int loop = 0, bool overwrite = true);

		// type conversion
		template <class T>
		T to() { return dynamic_cast<T>(this); }

		// debugging
		int id() const { return _id; }
		virtual std::string name() { return strprintf("Object[%d]", _id); }
};