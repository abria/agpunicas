// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SDL.h"
#include "View.h"
#include "Window.h"
#include "Game.h"
#include "Scene.h"
#include "Object.h"
#include "RenderableObject.h"
#include "timeUtils.h"

using namespace agp;

View::View(Scene* scene, const RectF& rect)
{
	_scene = scene;
	_rect = rect;
	_viewport = RectF(0.0f, 0.0f, 1.0f, 1.0f);
	_magf = PointF(1, 1);
	_aspectRatio = 0;
	updateViewport();
	_clipRect = RectF();
	_clipRectAbs = RectF();
}

void View::setScene(Scene* scene)
{
	_scene = scene;
	_rect = scene->rect();
}

void View::setRect(const RectF& r)
{ 
	_rect = r; 
	updateViewport(); 
}

void View::move(const Vec2Df& ds)
{
	_rect.pos += ds;
	updateViewport();
}

void View::move(float dx, float dy)
{
	_rect.pos.x += dx;
	_rect.pos.y += dy;
	updateViewport();
}

void View::scale(float f)
{
	_rect *= f;
	updateViewport();
}

void View::setPos(const Vec2Df& newPos)
{
	_rect.pos = newPos;
	updateViewport();
}

void View::render()
{
	SDL_Renderer* renderer = Game::instance()->window()->renderer();

	// viewport clipping
	SDL_Rect viewport_r = _viewportAbs.toSDL();
	SDL_Rect cliprect_r = _clipRectAbs.toSDL();
	if(_clipRectAbs.isValid())
		SDL_RenderSetClipRect(renderer, &cliprect_r);
	else
		SDL_RenderSetClipRect(renderer, &viewport_r);

	// viewport background
	SDL_SetRenderDrawColor(renderer, _scene->backgroundColor().r, _scene->backgroundColor().g, _scene->backgroundColor().b, _scene->backgroundColor().a);
	SDL_RenderFillRect(renderer, &viewport_r);

	// sort visible objects by z
	static Profiler viewRectProfiler("view rect object selection", 5000);
	viewRectProfiler.begin();
	auto objects = _scene->objects(_rect);
	viewRectProfiler.end();
	std::sort(objects.begin(), objects.end(),
		[](auto* a, auto* b) { return a->layer() < b->layer(); });

	// render objects
	for (auto& obj : objects)
	{
		RenderableObject* robj = obj->to<RenderableObject*>();
		if (robj)
			robj->draw(renderer, _scene2view);
	}
}

void View::updateViewport()
{
	SDL_Renderer* renderer = Game::instance()->window()->renderer();

	// get renderer size on screen
	int rendWidth, rendHeight;
	SDL_GetRendererOutputSize(renderer, &rendWidth, &rendHeight);

	// update viewport absolute coordinates
	_viewportAbs = RectF(
		_viewport.pos.x * rendWidth,
		_viewport.pos.y * rendHeight,
		_viewport.size.x * rendWidth,
		_viewport.size.y * rendHeight);

	// update cliprect
	if(_clipRect.isValid())
		_clipRectAbs = RectF(
			_clipRect.pos.x * rendWidth,
			_clipRect.pos.y * rendHeight,
			_clipRect.size.x * rendWidth,
			_clipRect.size.y * rendHeight);

	// correct aspect ratio
	if (_aspectRatio)
	{
		float currentRatio = _viewportAbs.size.x / _viewportAbs.size.y;
		if (currentRatio > _aspectRatio) // too wide
		{
			float newWidth = _viewportAbs.size.y * _aspectRatio;
			_viewportAbs.pos.x += (_viewportAbs.size.x - newWidth) / 2;
			_viewportAbs.size.x = newWidth;
		}
		else // too tall
		{
			float newHeight = _viewportAbs.size.x / _aspectRatio;
			_viewportAbs.pos.y += (_viewportAbs.size.y - newHeight) / 2;
			_viewportAbs.size.y = newHeight;
		}
	}

	// update magnification factor
	_magf.x = _viewportAbs.size.x / _rect.size.x;
	_magf.y = _viewportAbs.size.y / _rect.size.y;

	// update transforms
	_scene2view = [this](const PointF& p)
		{
			if (_rect.yUp)
				return PointF(
					_viewportAbs.pos.x + (p.x - _rect.pos.x) * _magf.x,
					_viewportAbs.pos.y - (p.y - _rect.pos.y - _rect.size.y) * _magf.y);
			else
				return PointF(
					_viewportAbs.pos.x + (p.x - _rect.pos.x) * _magf.x,
					_viewportAbs.pos.y + (p.y - _rect.pos.y) * _magf.y);
		};

	_view2scene = [this](const PointF& p)
		{
			if (_rect.yUp)
				return PointF(
					(p.x - _viewportAbs.pos.x) / _magf.x + _rect.pos.x,
					-(p.y - _viewportAbs.pos.y) / _magf.y + _rect.pos.y + _rect.size.y);
			else
				return PointF(
					(p.x - _viewportAbs.pos.x) / _magf.x + _rect.pos.x,
					(p.y - _viewportAbs.pos.y) / _magf.y + _rect.pos.y);
		};
}

PointF View::mapToScene(const PointF& p)
{
	return _view2scene(p);
}

PointF View::mapFromScene(const PointF& p)
{
	return _scene2view(p);
}

PointF View::mapToScene(float x, float y)
{
	return _view2scene(PointF(x, y));
}

PointF View::mapToScene(int x, int y)
{
	return mapToScene(float(x), float(y));
}

PointF View::mapFromScene(float x, float y)
{
	return _scene2view(PointF(x, y));
}

RectF View::mapToScene(const RectF& r)
{
	return RectF(
		(r.pos.x - _viewportAbs.pos.x) / _magf.x + _rect.pos.x,
		(_rect.yUp ? -1 : 1) * (r.pos.y - _viewportAbs.pos.y) / _magf.y + _rect.pos.y,
		r.size.x / _magf.x,
		r.size.y / _magf.y);
}

RectF View::mapFromScene(const RectF& r)
{
	return RectF(
		_viewportAbs.pos.x + (r.pos.x - _rect.pos.x) * _magf.x,
		_viewportAbs.pos.y + (_rect.yUp ? -1 : 1) * (r.pos.y - _rect.pos.y) * _magf.y,
		r.size.x * _magf.x,
		r.size.y * _magf.y);
}