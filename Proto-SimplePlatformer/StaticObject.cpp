// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "StaticObject.h"
#include "Scene.h"

using namespace agp;

StaticObject::StaticObject(Scene* scene, const RectF& rect, Sprite* sprite, int layer) :
	CollidableObject(scene, rect, sprite, layer)
{	

}

bool StaticObject::collision(CollidableObject* with, Direction fromDir)
{
	//_focused = true;
	
	return true;
}
