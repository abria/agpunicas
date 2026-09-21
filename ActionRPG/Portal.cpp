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
#include "RPGGameScene.h"

using namespace agp;

Portal::Portal(Scene* scene, const RotatedRectF& rrect, Portal* dest) :
	Trigger(scene, rrect, dynamic_cast<GameScene*>(scene)->player()->to<CollidableObject*>(), [](){})
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
			dynamic_cast<RPGGameScene*>(_scene)->setTransitionEnter(false);
			dynamic_cast<RPGGameScene*>(_scene)->setTransitionExit(true);
			schedule("transition_end", 1, [this]() {
				dynamic_cast<RPGGameScene*>(_scene)->setTransitionExit(false);
				dynamic_cast<GameScene*>(_scene)->player()->setFreezed(false);
				});
		};
}

bool Portal::collision(CollidableObject* with, bool begin, const Vec2Df& normal)
{
	if (with == _watched && _playerArrived && begin == false)
		_playerArrived = false;
	else if (with == _watched && !_playerArrived && begin)
	{
		dynamic_cast<GameScene*>(_scene)->player()->setFreezed(true);
		dynamic_cast<RPGGameScene*>(_scene)->setTransitionEnter(true);
		schedule("postponed_teleport", 1, [this]() {
			_task();
			}, 0, false);
	}
	
	return true;
}