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
#include "box2d/box2d.h"

namespace agp
{
	class GameScene;
	class OverlayScene;
	class Player;
}

// GameScene (or World) class
// - specialized update(dt) to semifixed timestep
// - may provide more efficient access to game objects (e.g. quadtree)
class agp::GameScene : public Scene
{
	private:

		float _dt;					// time integration step
		float _timeToSimulate;		// time integration extent
		int _subStepCount = 4;		// Box2D physics substep
		b2WorldId _worldId;			// Box2D world handle
		bool _collidersVisible;

		// scene overlays
		std::vector < OverlayScene*> _backgroundScenes;
		std::vector < OverlayScene*> _foregroundScenes;

		// player and controls
		Player* _player;
		bool _right_pressed;
		bool _left_pressed;
		bool _jump_pressed;
		bool _run_pressed;
		Vec2Df _cameraTranslateVel;	// camera scroll velocity (in scene units)
		float _cameraZoomVel;		// camera zoom velocity (in [0,1] relative scale units)

	public:

		GameScene(const RectF& r, float dt);
		virtual ~GameScene();

		// getters / setters
		b2WorldId worldId() { return _worldId; }
		Player* player() { return _player; }
		void setPlayer(Player* player) { _player = player; }
		bool collidersVisible() { return _collidersVisible; }
		void toggleColliders() { _collidersVisible = !_collidersVisible; }
		void addBackgroundScene(OverlayScene* bgScene) { _backgroundScenes.push_back(bgScene); }
		void addForegroundScene(OverlayScene* fgScene) { _foregroundScenes.push_back(fgScene); }

		// overrides scene object selection (+octree, NOT implemented)
		//virtual std::list<Object*> objects(const RectF& cullingRect) override;
	
		// overrides Scene's render (+overlay scenes)
		virtual void render();

		// implements game scene update logic
		virtual void update(float timeToSimulate) override;

		// extends event handler (+game controls)
		virtual void event(SDL_Event& evt) override;
};