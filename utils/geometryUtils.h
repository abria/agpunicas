// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2023 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include <ostream>
#include <string>
#include <cmath>
#include <functional>
#include <array>
#include <vector>
#include "SDL.h"

#ifdef BOX2D_UTILS
#include "box2d/box2d.h"
#endif

namespace agp
{
	// vector 2D (also point) class
	template <class T>
	struct Vec2D
	{
		// attributes
		T x = 0;
		T y = 0;

		// constructors
		Vec2D() : x(0), y(0) {}
		Vec2D(T _x, T _y) : x(_x), y(_y) {}
		Vec2D(const Vec2D& v) : x(v.x), y(v.y) {}

		// methods
		T mag() const { return T(std::sqrt(x * x + y * y)); }
		T mag2() const { return x * x + y * y; }
		T versX() const { return x ? x / std::abs(x) : 0; }
		T versY() const { return y ? y / std::abs(y) : 0; }
		Vec2D  norm() const { T r = 1 / mag(); return Vec2D(x * r, y * r); }
		Vec2D  perp(bool yUp = false) const { return yUp ? Vec2D(y, -x) : Vec2D(-y, x); } // clockwise
		Vec2D  floor() const { return Vec2D(std::floor(x), std::floor(y)); }
		Vec2D  ceil() const { return Vec2D(std::ceil(x), std::ceil(y)); }
		Vec2D  max(const Vec2D& v) const { return Vec2D(std::max(x, v.x), std::max(y, v.y)); }
		Vec2D  min(const Vec2D& v) const { return Vec2D(std::min(x, v.x), std::min(y, v.y)); }
		T dot(const Vec2D& v) const { return this->x * v.x + this->y * v.y; }
		T cross(const Vec2D& v) const { return this->x * v.y - this->y * v.x; }
		T distance(const Vec2D& p) { return std::sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y)); }
		
		// rotation by angle in radians w.r.t. horizontal axis, counterclockwise
		Vec2D rot(T angle, const Vec2D& center, bool yUp = false) const
		{
			if(yUp)
				return Vec2D(
					(x - center.x) * cos(angle) - (y - center.y) * sin(angle) + center.x,
				    (x - center.x) * sin(angle) + (y - center.y) * cos(angle) + center.y
				);
			else
				return Vec2D(
					(x - center.x) * cos(angle) + (y - center.y) * sin(angle) + center.x,
				   -(x - center.x) * sin(angle) + (y - center.y) * cos(angle) + center.y
				);
		}

		// string
		const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const Vec2D& v) { os << v.str(); return os; }

		// conversions
		inline SDL_FPoint toSDLf() const
		{
			return SDL_FPoint{ float(x), float(y)};
		}
		inline SDL_Point toSDL() const
		{
			return SDL_Point{ int(x), int(y) };
		}

#ifdef BOX2D_UTILS
		inline b2Vec2 toB2() const
		{
			return { x, y };
		}
#endif

		// operator overloads
		Vec2D& operator=(const Vec2D& v) = default;
		Vec2D  operator +  (const Vec2D& v) const { return Vec2D(this->x + v.x, this->y + v.y); }
		Vec2D  operator -  (const Vec2D& v) const { return Vec2D(this->x - v.x, this->y - v.y); }
		Vec2D  operator *  (const T& v)     const { return Vec2D(this->x * v, this->y * v); }
		Vec2D  operator *  (const Vec2D& v) const { return Vec2D(this->x * v.x, this->y * v.y); }
		Vec2D  operator /  (const T& v)     const { return Vec2D(this->x / v, this->y / v); }
		Vec2D  operator /  (const Vec2D& v) const { return Vec2D(this->x / v.x, this->y / v.y); }
		Vec2D& operator += (const Vec2D& v) { this->x += v.x; this->y += v.y; return *this; }
		Vec2D& operator -= (const Vec2D& v) { this->x -= v.x; this->y -= v.y; return *this; }
		Vec2D& operator *= (const T& v) { this->x *= v; this->y *= v; return *this; }
		Vec2D& operator /= (const T& v) { this->x /= v; this->y /= v; return *this; }
		Vec2D& operator *= (const Vec2D& v) { this->x *= v.x; this->y *= v.y; return *this; }
		Vec2D& operator /= (const Vec2D& v) { this->x /= v.x; this->y /= v.y; return *this; }
		Vec2D  operator +  () const { return { +x, +y }; }
		Vec2D  operator -  () const { return { -x, -y }; }
		bool operator == (const Vec2D& v) const { return (this->x == v.x && this->y == v.y); }
		bool operator < (const Vec2D& v) const { return sqrt(x * x + y * y) < sqrt(v.x * v.x + v.y * v.y); }
		bool operator != (const Vec2D& v) const { return (this->x != v.x || this->y != v.y); }

		// casting operators
		operator Vec2D<int32_t>() const { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
		operator Vec2D<float>() const { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
		operator Vec2D<double>() const { return { static_cast<double>(this->x), static_cast<double>(this->y) }; }
	};

	template<class T> inline Vec2D<T> operator * (const float& lhs, const Vec2D<T>& v)
	{
		return Vec2D<T>((T)(lhs * (float)v.x), (T)(lhs * (float)v.y));
	}
	template<class T> inline Vec2D<T> operator * (const double& lhs, const Vec2D<T>& v)
	{
		return Vec2D<T>((T)(lhs * (double)v.x), (T)(lhs * (double)v.y));
	}
	template<class T> inline Vec2D<T> operator * (const int& lhs, const Vec2D<T>& v)
	{
		return Vec2D<T>((T)(lhs * (int)v.x), (T)(lhs * (int)v.y));
	}
	template<class T> inline Vec2D<T> operator / (const float& lhs, const Vec2D<T>& v)
	{
		return Vec2D<T>((T)(lhs / (float)v.x), (T)(lhs / (float)v.y));
	}
	template<class T> inline Vec2D<T> operator / (const double& lhs, const Vec2D<T>& v)
	{
		return Vec2D<T>((T)(lhs / (double)v.x), (T)(lhs / (double)v.y));
	}
	template<class T> inline Vec2D<T> operator / (const int& lhs, const Vec2D<T>& v)
	{
		return Vec2D<T>((T)(lhs / (int)v.x), (T)(lhs / (int)v.y));
	}
	typedef Vec2D<float> Vec2Df;
	typedef Vec2Df PointF;
	typedef Vec2D<int> Point;

	
	// rectangle class
	template <class T> 
	struct Rect
	{
		// attributes
		Vec2D<T> pos;		// upper-left (yUp = false) or bottom-left (yUp = true)
		Vec2D<T> size;
		bool yUp;			// whether y-axis is upwards or downwards

		// constructors
		Rect() : pos(0, 0), size(0, 0), yUp(false) {}
		Rect(T x, T y, T width, T height, bool yUpwards=false) : pos(x, y), size(width, height), yUp(yUpwards){}
		Rect(const Vec2D<T>& v1, const Vec2D<T>& v2, bool yUpwards = false) : pos(v1), size(v2-v1), yUp(yUpwards) {}
		Rect(const Rect& r) : pos(r.pos), size(r.size), yUp(r.yUp) {}

		// casting (defined outside)
		template<typename U>
		Rect(const Rect<U>& other);

		// special getters
		inline T top() const { return yUp ? T(pos.y + size.y) : T(pos.y); }
		inline T right() const { return T(pos.x + size.x); }
		inline T bottom() const { return yUp ? T(pos.y) : T(pos.y + size.y); }
		inline T left() const { return T(pos.x); }
		inline T area() const { return size.x * size.y; }
		inline Vec2D<T> tl() const { return Vec2D<T>(left(), top()); }
		inline Vec2D<T> tr() const { return Vec2D<T>(right(), top()); }
		inline Vec2D<T> br() const { return Vec2D<T>(right(), bottom()); }
		inline Vec2D<T> bl() const { return Vec2D<T>(left(), bottom()); }
		inline std::array < Vec2D<T>, 4> vertices() const // counterclockwise starting from pos
		{
			if(yUp)
				return {
					Vec2D<T>(left(), bottom()),
					Vec2D<T>(right(), bottom()),
					Vec2D<T>(right(), top()),
					Vec2D<T>(left(), top()) };
			else
				return {
					Vec2D<T>(left(), top()),
					Vec2D<T>(left(), bottom()),
					Vec2D<T>(right(), bottom()),
					Vec2D<T>(right(), top()) };
		}

		// string
		const std::string str() const { return std::string("R[") + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(size.x) + ", " + std::to_string(size.y) + "]" + "(yUp=" + (yUp ? "true)" : "false)"); }
		friend std::ostream& operator << (std::ostream& os, const Rect& r) { os << r.str(); return os; }

		// operator overloads
		Rect& operator =  (const Rect& r) = default;
		Rect  operator +  (const T& s) const { return Rect(pos.x + s, pos.y + s, size.x, size.y, yUp); }
		Rect  operator +  (const Vec2D<T>& p) const { return Rect(pos.x + p.x, pos.y + p.y, size.x, size.y, yUp); }
		Rect  operator -  (const T& s) const { return Rect(pos.x - s, pos.y - s, size.x, size.y, yUp); }
		Rect  operator -  (const Vec2D<T>& p) const { return Rect(pos.x - p.x, pos.y - p.y, size.x, size.y, yUp); }
		Rect  operator *  (const T& s) const { return Rect(pos.x, pos.y, size.x * s, size.y * s, yUp); }
		Rect  operator *  (const Vec2D<T>& s) const { return Rect(pos.x, pos.y, size.x * s.x, size.y * s.y, yUp); }
		Rect  operator /  (const T& s) const { return Rect(pos.x, pos.y, size.x / s, size.y / s, yUp); }
		Rect  operator /  (const Vec2D<T>& s) const { return Rect(pos.x, pos.y, size.x / s.x, size.y / s.y, yUp); }
		Rect& operator += (const T& s) { pos.x += s; pos.y += s; return *this; }
		Rect& operator -= (const T& s) { pos.x -= s; pos.y -= s; return *this; }
		Rect& operator *= (const T& s) { size.x *= s; size.y *= s; return *this; }

		// operations
		inline bool isValid() const { return size.x > 0 && size.y > 0; }
		inline bool intersects(const Rect& r) const 
		{ 
			if (yUp)
			{
				return
					left() < r.right() && right() > r.left() &&
					bottom() < r.top() && top() > r.bottom();
			}
			else
			{
				return
					left() < r.right() && right() > r.left() &&
					top() < r.bottom() && bottom() > r.top(); 
			}
		}
		inline bool contains(const Vec2D<T> & p) const
		{
			if (yUp)
				return p.x > left() && p.x < right() && p.y > bottom() && p.y < top();
			else
				return p.x > left() && p.x < right() && p.y > top() && p.y < bottom();
		}
		Vec2D<T> center() const
		{
			return Vec2D<T>(pos.x + size.x / 2, pos.y + size.y / 2);
		}
		inline Rect united(const Rect& rect) const
		{
			if (yUp)
				return Rect(
					{ std::min(left(), rect.left()), std::min(bottom(), rect.bottom()) },
					{ std::max(right(), rect.right()), std::max(top(), rect.top()) });
			else
				return Rect(
					{ std::min(left(), rect.left()), std::min(top(), rect.top()) },
					{ std::max(right(), rect.right()), std::max(bottom(), rect.bottom()) });
		}
		inline void adjust(T dx1, T dy1, T dx2, T dy2)
		{
			pos.x += dx1;
			pos.y += dy1;
			size.x += dx2 - dx1;
			size.y += dy2 - dy1;
		}
		float aspectRatio() const
		{
			return float(size.x) / float(size.y);
		}

		// conversions
		inline SDL_FRect toSDLf() const
		{
			return SDL_FRect{ float(pos.x), float( pos.y), float(size.x), float(size.y) };
		}
		inline SDL_Rect toSDL() const
		{
			return SDL_Rect{ int(pos.x), int(pos.y), int(size.x), int(size.y) };
		}
	};

	// rect casting
	template <class T>
	template <class U>
	Rect<T>::Rect(const Rect<U>& other)
		: pos(other.pos), size(other.size), yUp(other.yUp) {}


	// line class
	template <class T>
	struct Line
	{
		// attributes
		Vec2D<T> start;
		Vec2D<T> end;

		// constructors
		Line() : start(0, 0), end(0, 0) {}
		Line(T x1, T y1, T x2, T y2) : start(x1, y1), end(x2, y2) {}
		Line(const Vec2D<T>& p1, const Vec2D<T>& p2) : start(p1), end(p2) {}
		Line(const Line& l) : start(l.start), end(l.end) {}

		// special getters
		Rect<T> boundingRect() const
		{
			return Rect<T>(
				{ std::min(start.x, end.x),std::min(start.y, end.y) },
				{ std::max(start.x, end.x),std::max(start.y, end.y) });
		}
	};
	typedef Line<float> LineF;


	// rotated rectangle class
	template <class T>
	struct RotatedRect
	{
		// attributes
		Vec2Df center;
		Vec2Df size;
		T angle;		// in radians w.r.t. horizontal axis, counterclockwise
		bool yUp;		// whether y-axis is upwards or downwards

		// constructors
		RotatedRect() : center(0, 0), size(0, 0), angle(0), yUp(false) {}
		RotatedRect(const Vec2D<T>& c, const Vec2D<T>& s, T ang = 0, bool yUpwards = false) : center(c), size(s), angle(ang), yUp(yUpwards) {}
		RotatedRect(T cx, T cy, T width, T height, T ang = 0, bool yUpwards = false) : center(cx, cy), size(width, height), angle(ang), yUp(yUpwards) {}
		RotatedRect(const Line<T>& upperEdge, T height, bool yUpwards = false)
		{
			Vec2D<T> upperEdgeVec = upperEdge.end - upperEdge.start;
			size = { upperEdgeVec.mag(), height };
			Vec2D<T> normal = upperEdgeVec.perp(yUpwards).norm();
			center = upperEdge.start + (upperEdgeVec / 2) + (height / 2) * normal;
			angle = std::atan2f(upperEdgeVec.y, upperEdgeVec.x);
		}
		RotatedRect(const RotatedRect& r) : center(r.center), size(r.size), angle(r.angle), yUp(r.yUp) {}

		// special getters
		inline std::array < Vec2D<T>, 4> vertices() const
		{ 
			Rect<T> rect(center - size / 2, center + size / 2);
			std::array < Vec2D<T>, 4> points = rect.vertices();
			std::array < Vec2D<T>, 4> transformed_points;
			for (int i = 0; i < 4; i++)
				transformed_points[i] = points[i].rot(angle, center);
			return transformed_points;
		}
		inline std::vector < Vec2D<T> > verticesVec() const
		{
			std::array < Vec2D<T>, 4> vertices_arr = vertices();
			return std::vector < Vec2D<T> >(vertices_arr.begin(), vertices_arr.end());
		}

#ifdef BOX2D_UTILS
		// capsule conversion
		b2Capsule toCapsule()
		{
			b2Capsule capsule;
			capsule.radius = size.x / 2;
			capsule.center1 = (center - Vec2D<T>(0, size.y / 2 - capsule.radius)).toB2();
			capsule.center2 = (center + Vec2D<T>(0, size.y / 2 - capsule.radius)).toB2();
			return capsule;
		}
#endif

		// string
		const std::string str() const { return std::string("RR[") + std::to_string(center.x) + ", " + std::to_string(center.y) + ", " + std::to_string(size.x) + ", " + std::to_string(size.y) + " @" + std::to_string(angle) + " yUp=" + (yUp ? "true]" : "false]"); }
		friend std::ostream& operator << (std::ostream& os, const RotatedRect& r) { os << r.str(); return os; }

		// operator overloads
		RotatedRect& operator=(const RotatedRect& r) = default;
		RotatedRect  operator +  (const T& s) const { return RotatedRect(center.x + s, center.y + s, size.x, size.y, angle, yUp); }
		RotatedRect  operator +  (const Vec2D<T>& s) const { return RotatedRect(center.x + s.x, center.y + s.y, size.x, size.y, angle, yUp); }
		RotatedRect  operator -  (const T& s) const { return RotatedRect(center.x - s, center.y - s, size.x, size.y, angle, yUp); }
		RotatedRect  operator -  (const Vec2D<T>& s) const { return RotatedRect(center.x - s.x, center.y - s.y, size.x, size.y, angle, yUp); }
		RotatedRect  operator *  (const T& s) const { return RotatedRect(center.x, center.y, size.x * s, size.y * s, angle, yUp); }
		RotatedRect& operator += (const T& s) { center.x += s; center.y += s; return *this; }
		RotatedRect& operator -= (const T& s) { center.x -= s; center.y -= s; return *this; }
		RotatedRect& operator *= (const T& s) { size.x *= s; size.y *= s; return *this; }

		// operations
		Rect<T> toRect() const
		{
			return Rect<T>(center.x - size.x / 2, center.y - size.y / 2, size.x, size.y, yUp);
		}
		Rect<T> boundingRect() const
		{
			std::array < Vec2D<T>, 4> verts = vertices();
			T x_min = verts[0].x;
			T x_max = verts[0].x;
			T y_min = verts[0].y;
			T y_max = verts[0].y;
			for (int i = 1; i < 4; i++)
			{
				x_min = std::min(x_min, verts[i].x);
				x_max = std::max(x_max, verts[i].x);
				y_min = std::min(y_min, verts[i].y);
				y_max = std::max(y_max, verts[i].y);
			}
			return Rect<T>({ x_min, y_min }, { x_max, y_max }, yUp);
		}
		inline bool contains(const Vec2D<T>& p) const
		{
			// counterclockwise vertices (from top-left) a, b, c, d
			auto verts = vertices();
			Vec2D<T>& a = verts[0];
			Vec2D<T>& b = verts[1];
			Vec2D<T>& c = verts[2];
			Vec2D<T>& d = verts[3];

			// test each edge ab, bc, ... if counterclockwise w.r.t. ap, bp, ..., then p outside triangle
			if ((p - a).cross(b - a) < 0.0f)
				return false;
			if ((p - b).cross(c - b) < 0.0f)
				return false;
			if ((p - c).cross(d - c) < 0.0f)
				return false;
			if ((p - d).cross(a - d) < 0.0f)
				return false;

			// otherwise p must be in (or on) the rect 
			return true;
		}
		// not implemented. Suggestion: apply SAT
		//inline bool intersects(const RotatedRect& r) const
		//{

		//}
	};

	typedef Rect<float> RectF;
	typedef Rect<float> AABB;
	typedef Rect<int> RectI;
	typedef RotatedRect<float> RotatedRectF;
	typedef RotatedRect<float> OBB;
	typedef std::function< Vec2Df(const Vec2Df&) > Transform;

	
	// Axis-Aligned direction (Y-downwards)
	enum class Direction { RIGHT, LEFT, UP, DOWN, NONE };
	static Vec2Df dir2vec(Direction dir, bool yUp = false)
	{
		if (dir == Direction::RIGHT)
			return Vec2Df(1, 0);
		else if (dir == Direction::LEFT)
			return Vec2Df(-1, 0);
		else if (dir == Direction::UP)
			return Vec2Df(0, float(yUp ? 1 : -1));
		else if (dir == Direction::DOWN)
			return Vec2Df(0, float(yUp ? -1 : 1));
		else
			return Vec2Df(0, 0);
	}
	static std::string dir2str(Direction dir)
	{
		if (dir == Direction::RIGHT)
			return "RIGHT";
		else if (dir == Direction::LEFT)
			return "LEFT";
		else if (dir == Direction::UP)
			return "UP";
		else if (dir == Direction::DOWN)
			return "DOWN";
		else
			return "none";
	}
	static Direction inverse(Direction dir)
	{
		if (dir == Direction::RIGHT)
			return Direction::LEFT;
		else if (dir == Direction::LEFT)
			return Direction::RIGHT;
		else if (dir == Direction::UP)
			return Direction::DOWN;
		else if (dir == Direction::DOWN)
			return Direction::UP;
		else
			return Direction::NONE;
	}
	static Direction normal2dir(Vec2Df _normal, bool yUp = false)
	{
		if (_normal.x == 0 && (yUp ? _normal.y == -1 : _normal.y == 1))
			return Direction::DOWN;
		else if (_normal.x == 0 && (yUp ? _normal.y == 1 : _normal.y == -1))
			return Direction::UP;
		else if (_normal.x == 1 && _normal.y == 0)
			return Direction::RIGHT;
		else if (_normal.x == -1 && _normal.y == 0)
			return Direction::LEFT;
		else
			return Direction::NONE;
	}
}