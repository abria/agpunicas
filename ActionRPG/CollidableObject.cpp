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

using namespace agp;

CollidableObject::CollidableObject(Scene* scene, const RotatedRectF& rrect, Sprite* sprite, int layer) :
	MovableObject(scene, rrect, sprite, layer)
{
	_angle = rad2deg(rrect.angle);

	defaultCollider();

	// default collision: non compenetration
	_compenetrable = false;
	_collidable = true;
}


void CollidableObject::defaultCollider()
{
	// set collider to default (whole rrect)
	_collider = RotatedRectF(0, 0, _rrect.size.x, _rrect.size.y, _rrect.angle);
}

bool CollidableObject::intersectsRect(const RectF& r)
{
	if (!intersectsRectShallow(r))
		return false;

	Vec2Df axis;
	float depth;
	return checkCollisionSAT(sceneCollider().verticesVec(), r.verticesVec(), axis, depth);
}

bool CollidableObject::intersectsRectShallow(const RectF& r)
{
	return sceneCollider().boundingRect().intersects(r);
}

bool CollidableObject::intersectsLine(const LineF& line, float& tNear)
{
	return sceneCollider().intersectsLine(line.start, line.end, tNear);
}

void CollidableObject::forgetObject(Object* objectToRemove)
{
	auto other = dynamic_cast<CollidableObject*>(objectToRemove);
	if (!other)
		return;

	_contactsPrev.erase(other);
	if (_contacts.erase(other))
		notifyContact(other, Contact::END, Vec2Df());
}

void CollidableObject::update(float dt)
{
	MovableObject::update(dt);

	resolveCollisions();
	if (_detectsContacts)
		detectContacts();
}

RotatedRectF CollidableObject::sceneCollider() const
{
	return _collider + _rrect.center;
}

void CollidableObject::notifyContact(CollidableObject* with, Contact contact, const Vec2Df& normal)
{
	// Share the contact so the other detector cannot announce BEGIN/END twice.
	if (contact == Contact::END)
		with->_contacts.erase(this);
	else
		with->_contacts[this] = -normal;
	collision(with, contact, normal);
	if (contact != Contact::STAY || !with->_detectsContacts)
		with->collision(this, contact, -normal);
}

void CollidableObject::detectContacts()
{
	_contactsPrev = _contacts;
	_contacts.clear();
	// Expand in local coordinates: SAT still tests the rotated shape, not its bounds.
	RotatedRectF probe = sceneCollider();
	probe.size += Vec2Df(2 * _contactTolerance, 2 * _contactTolerance);
	if (_collidable && !_killed)
		for (auto item : _scene->objects(probe.boundingRect()))
		{
			auto other = item->to<CollidableObject*>();
			Vec2Df normal;
			float depth;
			if (other && other != this && !other->_killed && other->collidable()
				&& collidableWith(other) && other->collidableWith(this)
				&& checkCollisionSAT(probe.verticesVec(), other->sceneCollider().verticesVec(), normal, depth))
				_contacts[other] = normal;
		}

	// Present now: BEGIN if absent before, otherwise STAY.
	for (const auto& contact : _contacts)
		notifyContact(contact.first, _contactsPrev.count(contact.first) ? Contact::STAY : Contact::BEGIN, contact.second);
	// Present before but absent now: END.
	for (const auto& contact : _contactsPrev)
		if (!_contacts.count(contact.first))
			notifyContact(contact.first, Contact::END, Vec2Df());
}

bool CollidableObject::collision(CollidableObject* with, Contact contact, const Vec2Df& normal)
{
	//printf("%s collided with %s from %s\n", name().c_str(), with->name().c_str(), normal.str().c_str());
	return true; 
}

void CollidableObject::draw(SDL_Renderer* renderer, Transform camera)
{
	MovableObject::draw(renderer, camera);

	GameScene* gameScene = dynamic_cast<GameScene*>(_scene);
	if (gameScene && gameScene->collidersVisible())
	{
		std::array < PointF, 4> drawVertices = sceneCollider().vertices();
		for (int i = 0; i < 4; i++)
			drawVertices[i] = camera(drawVertices[i]);
		
		DrawThickPolygon(renderer, drawVertices.data(), 4, _colliderPixelThickness, _colliderColor);
	}
}

float CollidableObject::distance(CollidableObject* obj) const
{
	return sceneCollider().center.distance(obj->sceneCollider().center);
}