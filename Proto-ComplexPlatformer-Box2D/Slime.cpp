#include "Slime.h"
#include "SpriteFactory.h"

using namespace agp;

Slime::Slime(GameScene* scene, const PointF& pos)
	: Enemy(scene, RotatedRectF(pos.x, pos.y, 5, 4, 0, true), SpriteFactory::instance()->get("slime"))
{
	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.density = 1;
	shapeDef.friction = 1.0f;
	shapeDef.restitution = 0.0;
	shapeDef.enableContactEvents = true;
	b2Polygon boxDef = b2MakeOffsetBox(_obb.size.x / 4, _obb.size.y / 4, { 0, 0 }, b2MakeRot(0));
	b2CreatePolygonShape(_bodyId, &shapeDef, &boxDef);
}