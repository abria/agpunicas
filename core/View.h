// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "geometryUtils.h"

namespace agp
{
	class Scene;
	class View;
}

// View (or camera) class
// - a rectangular camera (view) installed on the game scene
// - renders scene objects through a viewport
// - only scene objects within the view's rect are drawn (culling)
// - handles scene2view and view2scene transforms
class agp::View
{
	private:

		RectF _rect;			// view rect in scene coords
		Scene* _scene;			// scene (or world)
		RectF _viewport;		// viewport in relative [0,1] window coords
		RectF _viewportAbs;		// viewport in absolute window coords
		PointF _magf;			// view rect to viewport ratio (magnification factor)			
		Transform _scene2view;	// scene 2 view transform		
		Transform _view2scene;	// view 2 scene transform
		float _aspectRatio;	// fixed width/height aspect ratio (0 = not fixed)

	public:

		// constructors
		View(Scene* scene, const RectF& rect);

		// getters/setters
		Scene* scene() { return _scene; }
		void setScene(Scene* scene);
		const RectF& rect() const { return _rect; }
		void setRect(const RectF& r) { _rect = r; updateViewport();}
		const RectF& viewport() const { return _viewport; }
		PointF magf() const { return _magf; }
		void setViewport(const RectF& r) { _viewport = r; updateViewport();}
		void setFixedAspectRatio(float ratio) { _aspectRatio = ratio; updateViewport(); }
		void setX(float x) { _rect.pos.x = x; }
		void setY(float y) { _rect.pos.y = y; }

		// render scene objects within view rect (culling)
		void render();

		// view transforms
		void move(const Vec2Df& ds);
		void move(float dx, float dy);
		void scale(float f);
		void setPos(const Vec2Df& newPos);

		// update viewport
		void updateViewport();

		// mapping to/from scene coords
		PointF mapToScene(const PointF& p);
		PointF mapFromScene(const PointF& p);
		PointF mapToScene(float x, float y);
		PointF mapToScene(int x, int y);
		PointF mapFromScene(float x, float y);
		RectF mapToScene(const RectF& r);
		RectF mapFromScene(const RectF& r);
};