// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "KinematicObject.h"

using namespace agp;

KinematicObject::KinematicObject(GameScene* scene, const RotatedRectF& obb, Sprite* sprite, int layer) :
	RigidObject(scene, obb, sprite, b2BodyType::b2_kinematicBody, layer)
{
	
}