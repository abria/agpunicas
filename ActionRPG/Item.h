// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

namespace agp
{
	class Item;
	class Sprite;
}

// Item
// - implements inventory item
class agp::Item
{
	public:

		// 5 x 4 matrix
		enum class Type {
			BOW, BOOMERANG, HOOKSHOT, BOMB, POWDER,
			FIREROD, ICEROD, BOMBOS, ETHER, QUAKE,
			LAMP, HAMMER, FLUTE, NET, BOOK,
			BOTTLE, CANESOMARIA, CANEBYRNA, CAPE, MIRROR
		};

	protected:

		Type _category;
		Sprite* _icon;
		Sprite* _label;

	public:

		Item(Type category);
		virtual ~Item() {};	

		Sprite* icon() const { return _icon; }
		Sprite* label() const { return _label; }
};