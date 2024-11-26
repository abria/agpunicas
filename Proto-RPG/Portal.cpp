// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Portal.h"
#include "GameScene.h"

using namespace agp;

Portal::Portal(Scene* scene, const RectF& rect, Portal* dest) :
	Trigger(scene, rect, dynamic_cast<GameScene*>(scene)->player()->to<CollidableObject*>(), [](){})
{
	_playerArrived = false;
	setDestination(dest);
}

void Portal::setDestination(Portal* dest)
{ 	
	if(dest)
		_task = [this, dest]()
		{
			_watched->setPos(dest->rect().center() - _watched->rect().size/2);
			dest->_playerArrived = true;
		};
}

bool Portal::collision(CollidableObject* with, bool begin, Direction fromDir)
{
	if (with == _watched && _playerArrived && begin == false)
		_playerArrived = false;
	else if(with == _watched && !_playerArrived && begin)
		_task();
	
	return true;
}