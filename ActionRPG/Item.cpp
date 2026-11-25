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

Item::Item(Type category)
{
	_category = category;
}

Sprite* Item::createIcon() const
{
	return SpriteFactory::instance()->get(strprintf("inventory_icon_%d", int(_category)));
}

Sprite* Item::createLabel() const
{
	return SpriteFactory::instance()->get(strprintf("inventory_label_%d", int(_category)));
}