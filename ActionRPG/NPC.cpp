// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "NPC.h"
#include "SpriteFactory.h"
#include "Game.h"
#include "DialogBox.h"

using namespace agp;

NPC::NPC(Scene* scene, const PointF& pos)
	: DynamicObject(scene, RectF(pos.x, pos.y, 1, 1.3f), nullptr, 2)
{
	_velMax = { 0, 0 };
	_sprite = SpriteFactory::instance()->get("npc_example");
}

void NPC::interact()
{
	// example dialog with options
	Game::instance()->pushScene(new DialogBox(
		"The wizard, Agahnim, has done... something to the other missing girls! "
		"Now only I remain...\n"
		"Agahnim has seized control of the castle and is now trying to open "
		"the seven wise men's seal.\n"
		"I am in the dungeon of the castle.\n"
		"Would you help me?",
		{
			DialogBox::Option("YES", []() {printf("You are a good guy\n"); }),
			DialogBox::Option("NO", []() {printf("You are a BAD guy\n"); }),
			DialogBox::Option("MAYBE", []() {printf("Take a final decision\n"); })
		}));
}
