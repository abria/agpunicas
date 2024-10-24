// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "Scene.h"

namespace agp
{
	class GameScene;
	class Mario;
}

// GameScene (or World) class
// - specialized update(dt) to semifixed timestep
// - may provide more efficient access to game objects (e.g. quadtree)
// - can/should be customized for the specific game to implement (here, supermariobros-like)
// - stores the main player and implements player controls
class agp::GameScene : public Scene
{
	private:

		float _dt;					// time integration step
		float _timeToSimulate;		// time integration extent

		Mario* _mario;
		bool _right_pressed;
		bool _left_pressed;
		bool _jump_pressed;
		bool _run_pressed;

		bool _collidersVisible;
	
		float _cameraZoomVel;		// camera zoom velocity (in [0,1] relative scale units)

	public:

		GameScene(const RectF& r, float dt);
		virtual ~GameScene() {};

		Mario* player() { return _mario; }
		void setPlayer(Mario* mario) { _mario = mario; }
		bool collidersVisible() { return _collidersVisible; }
		void toggleColliders() { _collidersVisible = !_collidersVisible; }

		// overrides scene object selection (+octree, NOT implemented)
		//virtual std::list<Object*> objects(const RectF& cullingRect) override;

		// implements game scene update logic (+semifixed timestep)
		virtual void update(float timeToSimulate) override;

		// extends event handler (+camera translate/zoom, +brick creation/destroy, +pause menu)
		virtual void event(SDL_Event& evt) override;
};