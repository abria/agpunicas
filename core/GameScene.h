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
#include "graphicsUtils.h"
#include "Quadtree.h"

namespace agp
{
	class GameScene;
	class OverlayScene;
	class RenderableObject;
}

// GameScene (or World) class
// - specialized update(dt) to semifixed timestep
// - provides more efficient access to game objects (quadtree)
// - can/should be subclassed for the specific game to implement 
// - stores the main player and implements basic controls
class agp::GameScene : public Scene
{
	friend class Pathfinding;

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
		bool _displayGameSceneOnly;
	
		// camera controls
		Vec2Df _cameraTranslateVel;
		float _cameraZoomVel;		// camera zoom velocity (in [0,1] relative scale units)

		// space partitioning
		Quadtree _quadtree;

		// helper functions
		virtual void updateOverlayScenes(float timeToSimulate);
		virtual void updateControls(float timeToSimulate);
		virtual void updateWorld(float timeToSimulate);
		virtual void updateCamera(float timeToSimulate);

	public:

		GameScene(const RectF& rect, const Point& pixelUnitSize, float dt);
		virtual ~GameScene() {}

		Object* player() { return _player; }
		virtual void setPlayer(Object* player) { _player = player; }
		bool collidersVisible() const { return _collidersVisible; }
		virtual void toggleColliders() { _collidersVisible = !_collidersVisible; }
		virtual void toggleCameraManual() {	_cameraManual = !_cameraManual;	}
		virtual void toggleCameraFollowsPlayer() { _cameraFollowsPlayer = !_cameraFollowsPlayer; }
		virtual void addBackgroundScene(OverlayScene* bgScene) { _backgroundScenes.push_back(bgScene); }
		virtual void addForegroundScene(OverlayScene* fgScene) { _foregroundScenes.push_back(fgScene); }
		virtual void displayGameSceneOnly(bool on) { _displayGameSceneOnly = on; }

		// override add/remove objects (+quadtree)
		virtual void newObject(Object* obj) override;
		virtual void killObject(Object* obj) override;

		// override geometric queries (+quadtree)
		virtual Objects objects() override;
		virtual Objects objects(const RectF& cullingRect) override;
		virtual Objects objects(const PointF& containPoint) override;
		virtual bool isEmpty(const RectF& rect) override;

		// override render (+overlay scenes)
		virtual void render() override;

		// implements game scene update logic (+overlay, controls, +integration, +camera)
		virtual void update(float timeToSimulate) override;

		// override event handler (+camera translate/zoom)
		virtual void event(SDL_Event& evt) override;

		// override object move event (+quadtree)
		virtual void objectMoved(Object* obj) override;
};