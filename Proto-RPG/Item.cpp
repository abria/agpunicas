// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Item.h"
#include "SpriteFactory.h"
#include "Sprite.h"
#include "stringUtils.h"

using namespace agp;

Item::Item(Type type)
{
	_type = type;
	_icon = SpriteFactory::instance()->get(strprintf("inventory_icon_%d", int(_type)));
	_label = SpriteFactory::instance()->get(strprintf("inventory_label_%d", int(_type)));
}