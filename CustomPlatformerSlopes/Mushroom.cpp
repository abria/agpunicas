#include "Mushroom.h"
#include "SpriteFactory.h"
#include "Audio.h"
#include "Mario.h"
#include "PlatformerGame.h"
#include "HUD.h"
#include "StaticObject.h"
#include "Points.h"

using namespace agp;

Mushroom::Mushroom(Scene* scene, const PointF& pos, bool isGreen, int layer) :
	Spawnable(scene, RectF(pos.x, pos.y, 1.0f, 1.0f), layer)
{
	_isGreen = isGreen;
	_compenetrable = true;

	if(_isGreen)
		_sprite = SpriteFactory::instance()->get("mushroom_green");
	else
		_sprite = SpriteFactory::instance()->get("mushroom_red");

	_yGravityForce = 0;
	_visible = false;

	Audio::instance()->playSound("Item Box");

	schedule("spawn", 0.4f, [this]() {
		_vel.y = -1;
		_visible = true;

		schedule("startmove", 1.01f, [this]() {
			defaultPhysics();
			_xVelMax = 3;
			_xMoveForce = 1000;
			_xDir = Direction::LEFT;
			_yGravityForce /= 2;
			});
		});
}

// @override: +collision with Mario
bool Mushroom::collision(CollidableObject* with, bool begin, Direction fromDir)
{
	Mario* mario = dynamic_cast<Mario*>(with);
	StaticObject* staticObj = dynamic_cast<StaticObject*>(with);

	if (begin && mario)
	{
		if (_isGreen)
		{
			dynamic_cast<PlatformerGame*>(Game::instance())->hud()->addLife();
			new Points(_scene, pos(), Points::Value(8));
		}
		else
			mario->powerup();

		_visible = false;
		kill();
	}
	else if (begin && staticObj && (fromDir == Direction::RIGHT || fromDir == Direction::LEFT))
		_xDir = inverse(_xDir);

	return true;
}