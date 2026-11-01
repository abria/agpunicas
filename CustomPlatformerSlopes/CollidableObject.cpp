// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "CollidableObject.h"
#include "Scene.h"
#include <list>
#include <algorithm>
#include "timeUtils.h"
#include "collisionUtils.h"
#include "sdlUtils.h"
#include "GameScene.h"
#include "DynamicObject.h"
#include "StaticObject.h"
#include "KinematicObject.h"

using namespace agp;

CollidableObject::CollidableObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	MovableObject(scene, rect, sprite, layer)
{
	defaultCollider();
	_movementStartCollider = sceneCollider();

	// default collision: non compenetration
	_compenetrable = false;
	_collidable = true;

	// default collision system: Continous Collision Detection (CCD)
	_CCD = true;

	_fallingPrev = false;
}

void CollidableObject::defaultCollider()
{
	_collider = { 0, 0, rect().size.x, rect().size.y};
}

void CollidableObject::setCCD(bool active)
{
	_CCD = active;
}

bool CollidableObject::grounded() const
{
	if (_CCD)
		return MovableObject::grounded();

	return _fallingPrev && !falling();
}

bool CollidableObject::falling() const
{
	// Falling is a state, also when landing on a Lift switches CCD off.
	return midair() && _vel.y > 0;
}

bool CollidableObject::midair() const
{
	if (_CCD)
		return MovableObject::midair();

	// check for collisions with Static or Kinematic below current object
	for (const auto& contact : _contacts)
		if ((contact.first->to<StaticObject*>() || contact.first->to<KinematicObject*>())
			&& contact.second == Direction::DOWN)
			return false;

	return true;
}

void CollidableObject::forgetObject(Object* objectToRemove)
{
	auto other = dynamic_cast<CollidableObject*>(objectToRemove);
	if (!other)
		return;

	_contactsPrev.erase(other);
	if (_contacts.erase(other))
		notifyContact(other, Contact::END, Direction::NONE);
}

void CollidableObject::update(float dt)
{
	if (dt <= 0)
		return;
	MovableObject::update(dt);
	if (_freezedPhysics)
		return;

	_fallingPrev = falling();
	if (_detectsContacts)
	{
		_contactsPrev = _contacts;
		_contacts.clear();
	}
	// undo the free move before constraining it (e.g. to a slope surface)
	// prepareMovement() changes the velocity used by BOTH collision methods
	setPos(pos() - _vel * dt);
	_movementStartCollider = sceneCollider();
	prepareMovement(dt);
	if (_CCD)
	{
		resolveCollisionsCCD(dt);
		setPos(pos() + _vel * dt);
	}
	else
	{
		setPos(pos() + _vel * dt);
		resolveCollisionsAABB();
	}
	// finalize position, restore stored velocity and record any slope support
	finishMovement(dt);
	// notify contacts only after finishMovement() has registered support
	if (_detectsContacts)
		detectContacts();
}

bool CollidableObject::sweep(const RectF& body, const Vec2Df& movement,
	Vec2Df& point, Vec2Df& normal, float& time) const
{
	return DynamicRectVsRect(body, movement, sceneCollider(), point, normal, time);
}

bool CollidableObject::overlap(const RectF& body, Direction& axis, float& depth) const
{
	return checkCollisionAABB(body, sceneCollider(), axis, depth);
}

RectF CollidableObject::sceneCollider() const
{
	return _collider + rect().pos;
}

void CollidableObject::notifyContact(CollidableObject* with, Contact contact, Direction fromDir)
{
	// Share the contact so the other detector cannot announce BEGIN/END twice.
	if (contact == Contact::END)
		with->_contacts.erase(this);
	else
		with->_contacts[this] = inverse(fromDir);
	collision(with, contact, fromDir);
	if (contact != Contact::STAY || !with->_detectsContacts)
		with->collision(this, contact, inverse(fromDir));
}

void CollidableObject::detectContacts()
{
	// CCD already recorded crossed triggers; also find contacts at the final position.
	RectF probe = sceneCollider();
	probe.adjust(-_contactTolerance, -_contactTolerance, _contactTolerance, _contactTolerance);
	if (_collidable && !_killed)
		for (auto item : _scene->objects(probe))
		{
			auto other = item->to<CollidableObject*>();
			Direction side;
			float depth;
			if (other && other != this && !other->_killed && other->collidable() && collidableWith(other) && other->collidableWith(this)
				&& other->overlap(probe, side, depth))
				_contacts[other] = side;
		}

	// Present now: BEGIN if absent before, otherwise STAY.
	for (const auto& contact : _contacts)
		notifyContact(contact.first, _contactsPrev.count(contact.first) ? Contact::STAY : Contact::BEGIN, contact.second);
	// Present before but absent now: END.
	for (const auto& contact : _contactsPrev)
		if (!_contacts.count(contact.first))
			notifyContact(contact.first, Contact::END, Direction::NONE);
}

void CollidableObject::resolveCollisionsCCD(float dt)
{
	if (!_collidable || _killed)
		return;

	// NARROW collision detection
	// simulate next iteration pos to get objects within united bounding rect
	PointF curPos = pos();
	RectF curRect = sceneCollider();
	setPos(pos() + _vel * dt);
	std::vector<CollidableObject*> likely_collisions;
	Objects items_in_rect = _scene->objects(sceneCollider().united(curRect));
	for (auto item : items_in_rect)
	{
		CollidableObject* obj = item->to<CollidableObject*>();
		if (obj && obj != this && !obj->_killed && obj->collidable() && collidableWith(obj) && obj->collidableWith(this))
			likely_collisions.push_back(obj);
	}
	setPos(curPos);	// restore current pos

	// sort collisions in ascending order of contact time
	Vec2Df cp, cn;
	float ct = 0;
	std::vector<std::pair<CollidableObject*, float>> sortedByContactTime;
	for (auto& obj : likely_collisions)
		if (obj->sweep(sceneCollider(), vel() * dt, cp, cn, ct))
			sortedByContactTime.push_back({ obj, ct });
	std::sort(sortedByContactTime.begin(), sortedByContactTime.end(),
		[this](const std::pair<CollidableObject*, float>& a, const std::pair<CollidableObject*, float>& b)
		{
			// if contact time is the same, give priority to nearest object
			return a.second != b.second ? a.second < b.second : distance(a.first) < distance(b.first);
		});

	// Resolve solids first, then record triggers along the resulting movement.
	for (auto& obj : sortedByContactTime)
		if (!obj.first->compenetrable() && obj.first->sweep(sceneCollider(), vel() * dt, cp, cn, ct))
		{
			// Do not clamp slope-scaled velocity before finishMovement() restores it.
			_vel -= cn * cn.dot(_vel * (1 - std::max(0.0f, std::min(ct, 1.0f))));
			if (std::abs(_vel.y) < _yVelMin) _vel.y = 0;
			if (_detectsContacts) _contacts[obj.first] = inverse(normal2dir(cn));
		}
	for (auto obj : likely_collisions)
		if (_detectsContacts && obj->compenetrable() && obj->sweep(sceneCollider(), vel() * dt, cp, cn, ct))
			_contacts[obj] = inverse(normal2dir(cn));
}

void CollidableObject::resolveCollisionsAABB()
{
	if (!_collidable || _killed)
		return;

	// Detect penetrations at the same position, then apply the total correction.
	Vec2Df correction;
	for (auto item : _scene->objects(sceneCollider()))
	{
		auto other = item->to<CollidableObject*>();
		Direction side;
		float depth;
		if (other && other != this && !other->_killed && other->collidable() && collidableWith(other) && other->collidableWith(this)
			&& !other->compenetrable() && other->overlap(sceneCollider(), side, depth))
		{
			if (_detectsContacts) _contacts[other] = side;
			if (other->to<StaticObject*>() || other->to<KinematicObject*>())
				correction -= dir2vec(side) * depth;
			else if (other->to<DynamicObject*>())
				correction -= dir2vec(side) * depth / 10.0f;
		}
	}
	setPos(pos() + correction);
}

void CollidableObject::draw(SDL_Renderer* renderer, Transform camera)
{
	MovableObject::draw(renderer, camera);

	GameScene* gameScene = dynamic_cast<GameScene*>(_scene);
	if (gameScene && gameScene->collidersVisible())
	{
		auto vertices = sceneCollider().vertices();
		SDL_FRect drawRect = RectF(camera(vertices[0]), camera(vertices[2])).toSDLf();
		SDL_SetRenderDrawColor(renderer, _colliderColor.r, _colliderColor.g, _colliderColor.b, _colliderColor.a);
		DrawThickRect(renderer, drawRect, _colliderPixelThickness);
	}
}

float CollidableObject::distance(CollidableObject* obj) const
{
	return sceneCollider().center().distance(obj->sceneCollider().center());
}
