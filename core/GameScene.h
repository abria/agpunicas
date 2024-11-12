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
	class OverlayScene;
	class Link;
}

// GameScene (or World) class
// - specialized update(dt) to semifixed timestep
// - may provide more efficient access to game objects (e.g. quadtree)
// - can/should be subclassed for the specific game to implement 
// - stores the main player and implements basic controls
class agp::GameScene : public Scene
{
	protected:

		// basic physics/integration
		float _dt;					// time integration step
		float _timeToSimulateAccum;	// time to simulate (accumulator)

		// basic player controls
		Object* _player;
		bool _collidersVisible;
		bool _cameraManual;
		bool _cameraFollowsPlayer;

		// scene overlays
		std::vector < OverlayScene*> _backgroundScenes;
		std::vector < OverlayScene*> _foregroundScenes;
	
		// camera controls
		Vec2Df _cameraTranslateVel;
		float _cameraZoomVel;		// camera zoom velocity (in [0,1] relative scale units)

		// helper functions
		virtual void updateOverlayScenes(float timeToSimulate);
		virtual void updateControls(float timeToSimulate);
		virtual void updateWorld(float timeToSimulate);
		virtual void updateCamera(float timeToSimulate);

	public:

		GameScene(const RectF& rect, const Point& pixelUnitSize, float dt);
		virtual ~GameScene() {};

		Object* player() { return _player; }
		virtual void setPlayer(Object* player) { _player = player; }
		bool collidersVisible() const { return _collidersVisible; }
		void toggleColliders() { _collidersVisible = !_collidersVisible; }
		void toggleCameraManual() { _cameraManual = !_cameraManual; }
		void toggleCameraFollowsPlayer() { _cameraFollowsPlayer = !_cameraFollowsPlayer; }
		void addBackgroundScene(OverlayScene* bgScene) { _backgroundScenes.push_back(bgScene); }
		void addForegroundScene(OverlayScene* fgScene) { _foregroundScenes.push_back(fgScene); }

		// overrides scene object selection (+octree or +BVH, NOT implemented)
		//virtual std::list<Object*> objects(const RectF& cullingRect) override;

		// overrides Scene's render (+overlay scenes)
		virtual void render() override;

		// implements game scene update logic (+overlay, controls, +integration, +camera)
		virtual void update(float timeToSimulate) override;

		// extends event handler (+camera translate/zoom)
		virtual void event(SDL_Event& evt) override;
};