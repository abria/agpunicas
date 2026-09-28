// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2023 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

#include "geometryUtils.h"
#include "mathUtils.h"

namespace agp
{
	static inline bool checkCollisionAABB(
		const RectF& rectA, 
		const RectF& rectB, 
		Direction& collisionAxis, 
		float& collisionDepth)
	{
		if (!rectA.intersects(rectB))
			return false;

		float dx, dy;
		if (rectA.yUp)
		{
			dx = std::min(rectA.right(), rectB.right()) - std::max(rectA.left(), rectB.left());
			dy = std::min(rectA.top(), rectB.top()) - std::max(rectA.bottom(), rectB.bottom());
		}
		else
		{
			dx = std::min(rectA.right(), rectB.right()) - std::max(rectA.left(), rectB.left());
			dy = std::min(rectA.bottom(), rectB.bottom()) - std::max(rectA.top(), rectB.top());
		}

		if (dx < dy)
		{
			collisionDepth = dx;
			if (rectA.center().x < rectB.center().x)
				collisionAxis = Direction::RIGHT;
			else 
				collisionAxis = Direction::LEFT;
		}
		else
		{
			collisionDepth = dy;
			if (rectA.center().y < rectB.center().y)
				collisionAxis = rectA.yUp ? Direction::UP : Direction::DOWN;
			else
				collisionAxis = rectA.yUp ? Direction::DOWN : Direction::UP;
		}

		return true;
	}

	// SAT Polygon vs. Polygon collision detection with precomputed unit axes
	// polygon containers can be std::vector or std::array; touching counts as contact
	template <class PolygonA, class PolygonB, class Axes>
	static inline bool checkCollisionSAT(
		const PolygonA& polyA,
		const PolygonB& polyB,
		const Axes& axes,
		Vec2Df& collisionAxis,
		float& collisionDepth)
	{
		if (polyA.size() < 3 || polyB.size() < 3)
			return false;

		collisionDepth = inf<float>();
		for (const Vec2Df& axis : axes)
		{
			float minA = polyA[0].dot(axis), maxA = minA;
			float minB = polyB[0].dot(axis), maxB = minB;
			for (const Vec2Df& v : polyA)
			{
				const float projection = v.dot(axis);
				minA = std::min(minA, projection);
				maxA = std::max(maxA, projection);
			}
			for (const Vec2Df& v : polyB)
			{
				const float projection = v.dot(axis);
				minB = std::min(minB, projection);
				maxB = std::max(maxB, projection);
			}
			if (maxA < minB || maxB < minA)
				return false;

			const float depth = std::min(maxA - minB, maxB - minA);
			if (depth < collisionDepth)
			{
				collisionDepth = depth;
				collisionAxis = axis;
			}
		}

		// invert collisionAxis if not already going from A to B
		Vec2Df centerA, centerB;
		for (const Vec2Df& v : polyA) centerA += v;
		for (const Vec2Df& v : polyB) centerB += v;
		centerA /= float(polyA.size());
		centerB /= float(polyB.size());
		if ((centerB - centerA).dot(collisionAxis) < 0)
			collisionAxis = -collisionAxis;
		return true;
	}

	// SAT Polygon vs. Polygon collision detection
	// build axes from polygon edges; keep the nonzero-overlap test of this overload
	static inline bool checkCollisionSAT(
		const std::vector<Vec2Df>& polyA,
		const std::vector<Vec2Df>& polyB,
		Vec2Df& collisionAxis,
		float& collisionDepth)
	{
		std::vector<Vec2Df> axes;
		const std::vector<Vec2Df>* polygons[] = { &polyA, &polyB };
		for (const auto* poly : polygons)
			for (std::size_t i = 0; i < poly->size(); i++)
			{
				const Vec2Df edge = (*poly)[(i + 1) % poly->size()] - (*poly)[i];
				if (edge.mag2() > 0)
					axes.push_back(edge.perp().norm());
			}
		if (axes.empty())
			return false;
		return checkCollisionSAT(polyA, polyB, axes, collisionAxis, collisionDepth) && collisionDepth > 0;
	}

	// collision data for a single representative contact
	struct CollisionContact
	{
		Vec2Df normal;	// unit vector from A to B
		Vec2Df point;	// contact point in scene coordinates
		float depth = 0;
	};

	// closest point on a line segment
	static inline Vec2Df closestPointOnSegment(const Vec2Df& p, const Vec2Df& a, const Vec2Df& b)
	{
		const Vec2Df ab = b - a;
		if (ab.mag2() == 0)
			return a;
		const float t = std::max(0.0f, std::min(1.0f, (p - a).dot(ab) / ab.mag2()));
		return a + ab * t;
	}

	// OBB contact point from closest vertex-edge pairs
	// if two points have the same distance, use their midpoint
	static inline Vec2Df findContactPoint(
		const std::array<Vec2Df, 4>& polyA,
		const std::array<Vec2Df, 4>& polyB)
	{
		float minDistance = std::numeric_limits<float>::max();
		Vec2Df p1, p2;
		const std::array<Vec2Df, 4>* polygons[] = { &polyA, &polyB };
		for (int side = 0; side < 2; side++)
		{
			const auto& vertices = *polygons[side];
			const auto& edges = *polygons[1 - side];
			for (Vec2Df p : vertices)
				for (int j = 0; j < 4; j++)
				{
					const Vec2Df q = closestPointOnSegment(p, edges[j], edges[(j + 1) % 4]);
					const float distance = (p - q).mag2();
					if (distance < minDistance - 0.001f)
					{
						minDistance = distance;
						p1 = p2 = q;
					}
					else if (std::abs(distance - minDistance) <= 0.001f &&
						(q - p1).mag2() > (p2 - p1).mag2())
						p2 = q;
				}
		}
		return (p1 + p2) * 0.5f;
	}

	// Swept (CCD) Point vs. AABB collision detection
	static inline bool PointVsRect(
		const Vec2Df& p,
		const Vec2Df& vel,
		const RectF& target,
		Vec2Df& contact_point,
		Vec2Df& contact_normal,
		float& t_hit_near)
	{
		contact_normal = { 0,0 };
		contact_point = { 0,0 };

		// cache division
		// vel.x = 0 will yield +infinity, which is ok
		// <=> no intersection along x. Same applies to y
		Vec2Df inv_vel = 1.0 / vel;

		// calculate intersections with rectangle bounding axes
		Vec2Df t_near = (target.pos - p) * inv_vel;
		Vec2Df t_far = (target.pos + target.size - p) * inv_vel;

		// above lines may yield 0 * infinity = NaN, which is NOT ok
		// <=> one of the target sides touch velocity
		// <=> collision is NOT happening
		if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
		if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

		// swap distances if necessary
		if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
		if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

		// early collision rejection		
		if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

		// contact time will be the second t_near point along velocity ray
		t_hit_near = std::max(t_near.x, t_near.y);

		// exit contact time is the first t_far point along velocity ray
		float t_hit_far = std::min(t_far.x, t_far.y);

		// reject if velocity direction is pointing away from object
		if (t_hit_far < 0)
			return false;

		// contact point of collision from parametric line equation
		contact_point = p + t_hit_near * vel;

		// contact normal
		if (t_near.x > t_near.y)
			if (inv_vel.x < 0)
				contact_normal = { 1,  0 };
			else
				contact_normal = { -1,  0 };
		else
			if (inv_vel.y < 0)
				contact_normal = { 0,  1 };
			else
				contact_normal = { 0, -1 };

		// Note if t_near == t_far, collision is in a diagonal
		// we consider this case in the "else" branch which is
		// equivalent to assume that diagonal collisions
		// are resolved along the vertical axis
		return true;
	}

	// Swept (CCD) Dynamic AABB vs. AABB collision detection
	static inline bool DynamicRectVsRect(
		const RectF& source,
		const Vec2Df& source_vel,
		const RectF& target,
		Vec2Df& contact_point,
		Vec2Df& contact_normal,
		float& contact_time)
	{
		// expand target rectangle by source dimensions
		RectF expanded_target;
		expanded_target.pos = target.pos - source.size / 2;
		expanded_target.size = target.size + source.size;

		// perform ray vs rect intersection
		// * NOTE * due to limited precision, contact_time might be close to either 0 or 1
		// (e.g. 0-epsilon or 1+epsilon) even the *true* contact is really happening in [0,1]
		float epsilon = 0.001f;
		if (PointVsRect(source.pos + source.size / 2, source_vel, expanded_target, contact_point, contact_normal, contact_time))
		{
			contact_point -= 0.5 * source.size * contact_normal;
			return (contact_time >= 0 - epsilon && contact_time <= 1.0 + epsilon);
		}
		else
			return false;
	}

	// Swept (CCD) Point vs. Line collision detection
	static inline bool PointVsLine(
		const Vec2Df& p,
		const Vec2Df& vel,
		const LineF& target,
		Vec2Df& contact_point,
		Vec2Df& contact_normal,
		float& t_hit)
	{
		// Step 1: Find the parallel projection time of the point onto the line's normal

		// line endpoints
		Vec2Df line_start = target.start;
		Vec2Df line_end = target.end;

		// line's normal vector
		Vec2Df line_dir = (line_end - line_start).norm(); // make sure it's normalized
		Vec2Df line_normal = line_dir.perp(); // perpendicular to line direction

		// calculate time at which point's trajectory is parallel to line's normal
		float denom = vel.dot(line_normal);
		float epsilon = 0.000f;
		if (abs(denom) < epsilon) // no intersection or parallel movement
			return false; // no collision or line isn't moving towards the point

		float num = (line_start - p).dot(line_normal);
		t_hit = num / denom;

		if (t_hit < 0 - epsilon || t_hit > 1 + epsilon)
			return false; // intersection time must be within the sweep’s duration

		// Step 2: Find the actual intersection point
		contact_point = p + vel * t_hit;

		// Step 3: Check that the intersection point lies within the line segment
		float dotStart = (contact_point - line_start).dot(line_dir);
		float dotEnd = (contact_point - line_end).dot(line_dir);
		if (dotStart < 0 || dotEnd > 0)
			return false; // intersection point is not between start and end of the line segment

		// Step 4: Set the contact normal
		contact_normal = denom < 0 ? line_normal : -line_normal; // ensure normal points away from source

		// If we've got to this point, there is a valid intersection
		return true;
	}

	static inline bool DynamicLineVsLine(
		LineF lineA,
		Vec2Df velA,
		LineF lineB,
		Vec2Df& contact_point,
		Vec2Df& contact_normal,
		float& t_hit)
	{
		Vec2Df cps[4];
		Vec2Df cns[4];
		float ts[4];
		bool insersections[4];
		insersections[0] = PointVsLine(lineA.start,  velA, lineB, cps[0], cns[0], ts[0]);
		insersections[1] = PointVsLine(lineA.end,    velA, lineB, cps[1], cns[1], ts[1]);
		insersections[2] = PointVsLine(lineB.start, -velA, lineA, cps[2], cns[2], ts[2]);
		insersections[3] = PointVsLine(lineB.end,   -velA, lineA, cps[3], cns[3], ts[3]);
		t_hit = std::numeric_limits<float>::infinity();
		for(int i=0; i<4; i++)
			if (insersections[i] && ts[i] < t_hit)
			{
				t_hit = ts[i];
				contact_point = cps[i];
				contact_normal = cns[i];
			}
		return t_hit != std::numeric_limits<float>::infinity();
	}
}