// Regression checks for the port of SimplePlatformer-Slopes to SMB3/core.
#include "DynamicObject.h"
#include "Slope.h"
#include "Lift.h"
#include "Platform.h"
#include "Lift.h"
#include "Trigger.h"
#include "Scene.h"
#include "LevelData.h"
#include "LevelLoader.h"
#include "PlatformerGame.h"
#include "PlatformerGameScene.h"
#include "SpriteFactory.h"
#include "Mario.h"
#include "Game.h"
#include "Window.h"
#include "EditableObject.h"
#include "Audio.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <map>

using namespace agp;

void require(bool condition, const std::string& message)
{
	if (!condition) throw std::runtime_error(message);
}

// Keep the physical fixtures readable while constructing slopes from segments.
LineF slopeLine(const RectF& bounds, bool risesRight)
{
	return risesRight ? LineF(bounds.bl(), bounds.tr()) : LineF(bounds.tl(), bounds.br());
}

void segmentValidation()
{
	Scene scene({-20,-20,60,50},{16,16});
	for (LineF line : {LineF(0,0,0,0), LineF(0,0,0,2), LineF(0,0,2,1)})
	{
		bool rejected = false;
		try { new Platform(&scene, line); }
		catch (const std::runtime_error&) { rejected = true; }
		require(rejected, "Platform constructor accepted an invalid segment");
	}
	for (LineF line : {LineF(0,0,0,0), LineF(0,0,0,2), LineF(0,0,2,0)})
	{
		bool rejected = false;
		try { new Slope(&scene, line); }
		catch (const std::runtime_error&) { rejected = true; }
		require(rejected, "Slope constructor accepted an invalid segment");
	}
	new Platform(&scene, LineF(4,0,0,0));
	scene.update(0);
	require(scene.objects().size() == 1, "failed construction left objects in the scene");
}

class Body : public DynamicObject
{
public:
	std::map<CollidableObject*, int> exits;
	std::map<CollidableObject*, int> contacts;
	Body(Scene* scene, RectF rect) : DynamicObject(scene, rect, nullptr)
	{
		_xMoveForce = _xFrictionForce = _xSkiddingForce = 0;
		_xVelMax = 6;
		_yVelMax = 100;
	}
	void velocity(float x, float y = 0) { _vel = {x, y}; }
	void freezePhysics(bool on) { _freezedPhysics = on; }
	bool ccd() const { return _CCD; }
	void gravity(float value) { _yGravityForce = value; }
	void defaultForces(float maxSpeed = 6) { defaultPhysics(); _xVelMax = maxSpeed; }
	bool collision(CollidableObject* with, bool begin, Direction) override
	{
		if (!begin) exits[with]++;
		else contacts[with]++;
		return true;
	}
};

void objectRemoval()
{
	// Removing a frozen support must end its contact and restore CCD immediately.
	Scene scene({-20,-20,60,50},{16,16});
	auto* platform = new KinematicObject(&scene, {0,0,4,1}, nullptr);
	auto* body = new Body(&scene, {1,-1,0.6f,1});
	scene.update(0);
	body->setCCD(false);
	body->update(0.01f);
	require(body->contacts[platform] > 0, "removal: missing initial platform contact");
	int exits = body->exits[platform];
	body->setFreezed(true);
	platform->setFreezed(true);
	platform->kill();
	platform->kill(); // repeated kill must still delete only once
	scene.update(0);
	require(scene.objects().size() == 1 && body->exits[platform] == exits + 1,
		"removal: frozen support was not removed or contact did not end once");
	require(body->ccd(), "removal: losing the platform did not restore CCD");
	body->setFreezed(false);
	body->update(0.01f); // also exercises previous contacts after the deletion
	require(body->pos().y > -1, "removal: body did not fall after losing its support");

	// A frozen platform must forget its passengers even without updating itself.
	platform = new KinematicObject(&scene, {0,5,4,1}, nullptr);
	scene.update(0);
	platform->collision(body, true, Direction::UP);
	platform->setFreezed(true);
	body->kill();
	scene.update(0);
	platform->setFreezed(false);
	platform->update(0.01f); // AddressSanitizer catches a stale passenger here

	// Compenetrable contacts must also be safe to resume after removal.
	body = new Body(&scene, {-1,0,0.6f,1});
	body->gravity(0);
	body->velocity(3);
	int triggered = 0;
	auto* trigger = new Trigger(&scene, {0,0,4,2}, body, [&]() { ++triggered; });
	scene.update(0);
	for (int i = 0; i < 30; ++i) body->update(0.01f);
	require(triggered > 0, "removal: missing initial trigger contact");
	body->setFreezed(true);
	trigger->kill();
	scene.update(0);
	body->setFreezed(false);
	body->update(0.01f);

	// New/frozen objects and kills requested during notification need no extra tick.
	struct Probe : Object
	{
		int& destroyed;
		bool cascade;
		Probe(Scene* scene, int& count, bool cascade = false)
			: Object(scene, {0,0,1,1}), destroyed(count), cascade(cascade) {}
		~Probe() override { ++destroyed; }
		void forgetObject(Object*) override { if (cascade) kill(); }
	};
	int destroyed = 0;
	Scene pending({0,0,10,10},{16,16});
	auto* victim = new Probe(&pending, destroyed);
	new Probe(&pending, destroyed, true);
	victim->setFreezed(true);
	victim->kill();
	pending.update(0);
	require(destroyed == 2 && pending.objects().empty(), "removal: pending/cascaded kill was delayed");
}

void hill(bool reverse, bool ccd)
{
	Scene scene({-20,-20,60,50},{16,16});
	new StaticObject(&scene, {-10,0,40,3});
	auto* up = new Slope(&scene, slopeLine({0,-2,4,2}, true));
	new StaticObject(&scene, {4,-2,2,2});
	auto* down = new Slope(&scene, slopeLine({6,-2,4,2}, false));
	auto* body = new Body(&scene, {reverse ? 11.0f : -2.0f,-1,0.6f,1});
	body->velocity(reverse ? -3 : 3);
	body->move(reverse ? Direction::LEFT : Direction::RIGHT);
	body->setCCD(ccd);
	scene.update(0);
	for (int i=0;i<700;++i)
	{
		body->update(0.01f);
		const RectF r = body->sceneCollider();
		for (auto* slope : {up,down})
			if (slope->overlapsX(r))
				require(r.bottom() <= slope->surfaceY(r) + 0.015f, "hill: body penetrated a slope");
		require(std::abs(body->vel().x - (reverse ? -3 : 3)) < 0.01f, "hill: stored horizontal speed changed");
	}
	require(reverse ? body->pos().x < -3 : body->pos().x > 13, "hill: blocked at a seam, reverse=" + std::to_string(reverse) + " ccd=" + std::to_string(ccd) + " x=" + std::to_string(body->pos().x) + " y=" + std::to_string(body->pos().y));
	require(body->exits[up] == 1 && body->exits[down] == 1, "hill: incorrect slope contact lifecycle");
}

void valley(bool reverse)
{
	Scene scene({-20,-20,60,50},{16,16});
	new StaticObject(&scene, {-10,0,40,3});
	auto* left = new Slope(&scene, slopeLine({0,-3,4,3}, false));
	auto* right = new Slope(&scene, slopeLine({4,-3,4,3}, true));
	auto* body = new Body(&scene, {reverse ? 6.0f : 1.0f,-4,0.6f,1});
	body->setPos({body->pos().x, (reverse ? right : left)->surfaceY(body->sceneCollider())-1});
	body->velocity(reverse ? -3 : 3);
	body->move(reverse ? Direction::LEFT : Direction::RIGHT);
	scene.update(0);
	for (int i=0;i<260;++i)
	{
		body->update(0.01f);
		for (auto* slope : {left,right})
			if (slope->overlapsX(body->sceneCollider()))
				require(body->sceneCollider().bottom() <= slope->surfaceY(body->sceneCollider()) + 0.015f, "valley: penetration");
	}
	require(reverse ? body->pos().x < 3 : body->pos().x > 5, "valley: cannot cross bottom");
}

void speedAndJump()
{
	Scene scene({-20,-20,60,50},{16,16});
	auto* slope = new Slope(&scene, slopeLine({0,-4,8,4}, true));
	auto* body = new Body(&scene, {3,-3,0.6f,1});
	scene.update(0);
	for (float vx : {3.0f, -3.0f, 6.0f, -6.0f})
	{
		body->setPos({3, slope->surfaceY({3,0,0.6f,1})-1});
		body->velocity(vx);
		body->move(vx > 0 ? Direction::RIGHT : Direction::LEFT);
		body->update(0.01f);
		float expected = 3 + vx * (vx > 0 ? 1/1.5f : 1.5f) * 0.01f;
		require(std::abs(body->pos().x-expected) < 0.0001f, "incorrect slope speed factor");
		require(std::abs(body->vel().x-vx) < 0.0001f, "downhill speed changed stored velocity");
		require(!body->midair(), "not grounded on slope");
	}
	body->velocity(0,-10);
	body->update(0.01f);
	require(body->sceneCollider().bottom() < slope->surfaceY(body->sceneCollider()) - 0.05f, "jump stuck to ramp");
	body->setPos({3,-10});
	body->velocity(0,30);
	for(int i=0;i<60;++i) body->update(0.01f);
	require(std::abs(body->sceneCollider().bottom()-slope->surfaceY(body->sceneCollider())) < 0.001f, "fall did not land on ramp");
	PointF oldPos = body->pos();
	body->velocity(3);
	body->freezePhysics(true);
	body->update(0.01f);
	require(body->pos() == oldPos, "frozen physics moved on slope");
	body->freezePhysics(false);
	body->update(0);
	require(body->pos() == oldPos, "zero step moved body");
}

void blockers()
{
	Scene scene({-20,-20,60,50},{16,16});
	new Slope(&scene, slopeLine({0,-4,4,4}, true));
	auto* body = new Body(&scene, {1,-2.6f,0.6f,1});
	new StaticObject(&scene, {2,-6,0.5f,5});
	scene.update(0);
	body->velocity(3);
	body->move(Direction::RIGHT);
	for (int i=0;i<100;++i) body->update(0.01f);
	require(body->sceneCollider().right() <= 2.001f, "passed through wall on slope");
	new StaticObject(&scene, {0,-3,1.8f,0.2f});
	scene.update(0);
	body->setPos({1,-2.6f});
	body->velocity(3);
	body->move(Direction::RIGHT);
	for (int i=0;i<100;++i) body->update(0.01f);
	require(body->sceneCollider().top() >= -2.801f, "climbed through ceiling");
}

void openSlopeSides(bool ccd, bool risesRight)
{
	Scene scene({-20,-20,60,50},{16,16});
	auto* slope = new Slope(&scene, slopeLine({0,-4,4,4}, risesRight));
	auto* body = new Body(&scene, {0,0,0.6f,0.6f});
	body->gravity(0);
	body->setCCD(ccd);
	scene.update(0);
	// Cross each lateral edge and the base, below the inclined surface.
	const PointF starts[] = {{-0.8f,-0.5f}, {4.2f,-0.5f}, {2,0.2f}};
	const Vec2Df velocities[] = {{2,0}, {-2,0}, {0,-2}};
	for (int i = 0; i < 3; ++i)
	{
		body->setPos(starts[i]);
		body->velocity(velocities[i].x, velocities[i].y);
		body->update(0.3f);
		const PointF expected = starts[i] + velocities[i] * 0.3f;
		require(body->pos().distance(expected) < 0.0001f,
			"slope segment: an absent side blocked movement");
		require(body->contacts[slope] == 0,
			"slope segment: an absent side generated a contact");
	}
}

void platformAndTrigger()
{
	Scene scene({-20,-20,60,50},{16,16});
	new Slope(&scene, slopeLine({0,-2,4,2}, true));
	new Platform(&scene, LineF(0,-3,4,-3));
	auto* body = new Body(&scene, {1,-5,0.6f,1});
	int count = 0;
	new Trigger(&scene, {0,-3.9f,4,0.8f},body,[&count](){++count;});
	scene.update(0);
	for (int i=0;i<100;++i) body->update(0.01f);
	require(std::abs(body->sceneCollider().bottom()+3) < 0.01f, "ramp stole contact from a higher platform");
	require(count > 0, "trigger callback lost");
}

void oneWayPlatforms(bool ccd)
{
	Scene scene({-20,-20,60,50},{16,16});
	auto* platform = new Platform(&scene, LineF(0,0,4,0));
	auto* body = new Body(&scene, {1,-2,0.6f,1});
	body->setCCD(ccd);
	scene.update(0);
	for (int i=0; i<80; ++i) body->update(0.01f);
	require(std::abs(body->sceneCollider().bottom()) < 0.001f, "one-way: failed to land from above");

	// Jump all the way through, then land on the return journey.
	body->setPos({1,0.4f});
	body->velocity(0,-18);
	bool cleared = false;
	for (int i=0; i<60; ++i)
	{
		body->update(0.01f);
		cleared = cleared || body->sceneCollider().bottom() < -0.1f;
	}
	require(cleared && std::abs(body->sceneCollider().bottom()) < 0.001f,
		"one-way: jump from below or subsequent landing failed");

	// If only the head clears the segment, the body must fall back through.
	body->setPos({1,0.2f});
	body->velocity(0,-14);
	int contacts = body->contacts[platform];
	for (int i=0; i<40; ++i) body->update(0.01f);
	require(body->pos().y > 0.2f && body->contacts[platform] == contacts,
		"one-way: caught a partial jump from below");

	body->gravity(0);
	for (bool reverse : {false,true})
	{
		body->setPos({reverse ? 5.0f : -1.0f,-0.5f});
		body->velocity(reverse ? -3 : 3);
	body->move(reverse ? Direction::LEFT : Direction::RIGHT);
		contacts = body->contacts[platform];
		for (int i=0; i<220; ++i) body->update(0.01f);
		require((reverse ? body->pos().x < -1 : body->pos().x > 5) &&
			body->contacts[platform] == contacts, "one-way: side collision");
	}
}

void idleSliding(bool ccd)
{
	for (bool risesRight : {false,true})
	for (float gradient : {0.25f,0.5f,1.0f,1.5f,4.0f})
	{
		Scene scene({-50,-150,100,200},{16,16});
		auto* slope = new Slope(&scene, slopeLine({0,-20*gradient,20,20*gradient}, risesRight));
		auto* body = new Body(&scene, {10,0,0.6f,1});
		body->setPos({10,slope->surfaceY(body->sceneCollider())-1});
		body->setCCD(ccd);
		scene.update(0);
		for (int i=0; i<30; ++i) body->update(0.01f);
		float dx = body->pos().x-10;
		float surfaceSpeed = std::abs(dx)*std::sqrt(1+gradient*gradient)/0.3f;
		require(risesRight ? dx < 0 : dx > 0, "idle: did not slide downhill");
		require(std::abs(surfaceSpeed-std::abs(slope->slideVelocity())*std::sqrt(1+gradient*gradient)) < 0.002f,
			"idle: displacement disagrees with configured slide velocity");
		require(body->vel().x == 0 && !body->midair(), "idle: slide changed stored velocity or lost support");
	}

	Scene valley({-20,-20,60,50},{16,16});
	auto* left = new Slope(&valley, slopeLine({0,-3,4,3}, false));
	new Slope(&valley, slopeLine({4,-3,4,3}, true));
	auto* body = new Body(&valley, {1.5f,0,0.6f,1});
	body->setPos({1.5f,left->surfaceY(body->sceneCollider())-1});
	body->setCCD(ccd);
	valley.update(0);
	for (int i=0; i<800; ++i) body->update(0.01f);
	float restingX = body->pos().x;
	for (int i=0; i<100; ++i) body->update(0.01f);
	require(std::abs(restingX-3.7f) < 0.02f && std::abs(body->pos().x-restingX) < 0.0001f,
		"idle: did not settle at the bottom of the valley");
}

void uphillControl()
{
	float previousSpeed = INFINITY;
	for (float gradient : {0.5f,1.0f,2.0f})
	{
		Scene scene({-50,-150,100,200},{16,16});
		auto* slope = new Slope(&scene, slopeLine({0,-20*gradient,20,20*gradient}, true));
		auto* body = new Body(&scene, {10,0,0.6f,1});
		body->setPos({10,slope->surfaceY(body->sceneCollider())-1});
		body->defaultForces();
		body->move(Direction::RIGHT);
		scene.update(0);
		for (int i=0; i<120; ++i) body->update(0.01f);
		float before = body->pos().x;
		body->update(0.01f);
		float speed = (body->pos().x-before)/0.01f;
		require(speed > 0 && speed < previousSpeed && std::abs(speed-6/(1+gradient)) < 0.002f,
			"uphill: control cannot overcome slide or speed does not decrease with gradient");
		previousSpeed = speed;
		body->move(Direction::NONE);
		for (int i=0; i<100; ++i) body->update(0.01f);
		before = body->pos().x;
		for (int i=0; i<20; ++i) body->update(0.01f);
		require(body->pos().x < before, "uphill: release did not lead to passive sliding");
	}
}

void uphillRelease()
{
	for (bool ccd : {false,true})
	for (bool risesRight : {false,true})
	for (float gradient : {0.5f,1.0f,2.0f})
	for (float inputSpeed : {6.0f,13.0f})
	{
		Scene scene({-50,-150,100,200},{16,16});
		auto* slope = new Slope(&scene, slopeLine({0,-20*gradient,20,20*gradient}, risesRight));
		auto* body = new Body(&scene, {10,0,0.6f,1});
		body->setPos({10,slope->surfaceY(body->sceneCollider())-1});
		body->defaultForces(inputSpeed);
		body->setCCD(ccd);
		body->move(risesRight ? Direction::RIGHT : Direction::LEFT);
		body->velocity(risesRight ? inputSpeed : -inputSpeed);
		scene.update(0);
		body->update(0.01f);
		body->move(Direction::NONE);
		int firstSlide = 0;
		for (int i=1; i<=15; ++i)
		{
			float before = body->pos().x;
			body->update(0.01f);
			if (risesRight ? body->pos().x < before : body->pos().x > before)
			{
				firstSlide = i;
				break;
			}
		}
		require(firstSlide > 0, "uphill: release took more than 150 ms to start sliding");
		require(body->vel().x == 0 && !body->midair(), "uphill: release lost support or injected velocity");
	}
}

class TestGame : public PlatformerGame
{
public:
	PlatformerGameScene* world() { return dynamic_cast<PlatformerGameScene*>(_scenes.front()); }
};

void levelIntegration()
{
	auto* game = new TestGame();
	Game::setInstance(game);
	SpriteFactory::instance();
	Audio::instance();
	game->init();
	game->popScene(); // title menu
	auto* world = game->world();
	world->Scene::update(0);
	LevelData level(std::string(SDL_GetBasePath()) + "levels/1-1.json");
	int ramps = 0;
	for (auto* object : world->objects())
		if (auto* slope = object->to<Slope*>())
		{
			++ramps;
			require(slope->color().a == 0, "slope collider must be invisible");
		}
	require(ramps == 6, "level did not instantiate six slopes");
	require(world->backgroundImages().size() == 1, "missing editor background image");
	Audio::instance()->playMusic("overworld");

	// Exercise the actual Mario collider on the first hill.
	auto* mario = world->player()->to<Mario*>();
	mario->move(Direction::RIGHT);
	for (int i=0; i<220; ++i)
	{
		mario->update(0.01f);
		for (auto* object : world->objects())
		{
			auto* slope = object->to<Slope*>();
			if (slope && slope->overlapsX(mario->sceneCollider()))
				require(mario->sceneCollider().bottom() <= slope->surfaceY(mario->sceneCollider()) + 0.01f, "Mario penetrated slope");
		}
	}
	require(mario->pos().x > 8, "Mario failed to climb first hill");

	// Round-trip using the editor's actual object serialization.
	Scene editorObjects(world->rect(), world->pixelUnitSize());
	auto* lift = new Lift(&editorObjects, {0, 0, 3, 0.5f}, true, 3);
	require(lift->sprite() != nullptr, "Lift must load its own sprite");
	auto* collider = new StaticObject(&editorObjects, {0, 2, 3, 1});
	require(collider->sprite() == nullptr && collider->color().a == 0, "generic Static must be transparent");
	auto categories = level.json().at("categories").get<std::vector<std::string>>();
	auto saved = level.json();
	saved["objects"] = nlohmann::ordered_json::array();
	for (const auto& object : level.objects())
	{
		auto* editable = new EditableObject(&editorObjects, object, categories);
		auto serialized = editable->toJson();
		for (const char* key : {"multiline", "content", "layer"})
			if (object.contains(key)) require(serialized.at(key) == object.at(key), std::string("editor lost ") + key);
		saved["objects"].push_back(serialized);
	}
	std::string savedPath = std::string(SDL_GetBasePath()) + "levels/roundtrip-test.json";
	LevelData::save(savedPath, saved);
	LevelData roundtrip(savedPath);
	require(!roundtrip.json().contains("scene"), "editor data must not contain scene settings");
	delete LevelLoader::instance()->load("roundtrip-test");
	// Reversing every slope's endpoints must preserve its surface and downhill direction.
	for (auto& object : saved["objects"])
		if (level.category(object) == "Slope")
			std::reverse(object["multiline"].begin(), object["multiline"].end());
	LevelData::save(savedPath, saved);
	auto* reversed = LevelLoader::instance()->load("roundtrip-test");
	reversed->update(0);
	int matched = 0;
	for (auto* object : reversed->objects())
		if (auto* slope = object->to<Slope*>())
			for (auto* original : world->objects())
				if (auto* expected = original->to<Slope*>())
					if (slope->rect().pos == expected->rect().pos && slope->rect().size == expected->rect().size)
					{
						require(slope->surfaceY(mario->sceneCollider()) == expected->surfaceY(mario->sceneCollider()), "reversing endpoints changed slope surface");
						require(slope->slideVelocity() == expected->slideVelocity(), "reversing endpoints changed downhill direction");
						++matched;
					}
	require(matched == 6, "reversed slopes did not match original geometry");
	delete reversed;

	// Reject flat, vertical and zero-length ramps, rather than dividing by zero.
	for (int kind = 0; kind < 3; ++kind)
	{
		auto invalid = saved;
		for (auto& object : invalid["objects"])
			if (level.category(object) == "Slope")
			{
				auto& points = object["multiline"];
				if (kind != 0) points[1]["x"] = points[0]["x"];
				if (kind != 1) points[1]["y"] = points[0]["y"];
				break;
			}
		LevelData::save(savedPath, invalid);
		bool rejected = false;
		try { delete LevelLoader::instance()->load("roundtrip-test"); }
		catch (const std::runtime_error&) { rejected = true; }
		require(rejected, "invalid slope segment was accepted");
	}
	std::remove(savedPath.c_str());

	// Render an actual game frame for visual inspection.
	mario->setPos({2.5625f, 0});
	world->Scene::update(0);
	world->render();
	SDL_Surface* frame = SDL_RenderReadPixels(game->window()->renderer(), nullptr);
	require(frame != nullptr, "could not render game frame");
	IMG_SavePNG(frame, (std::string(SDL_GetBasePath()) + "slopes-preview.png").c_str());
	SDL_DestroySurface(frame);
	std::cout << "Level, Mario, assets and editor round-trip checks passed\n";
}

int main(int argc, char** argv)
{
	try
	{
		if (argc > 1 && std::string(argv[1]) == "--level")
		{
			levelIntegration();
			return 0;
		}
		segmentValidation();
		for (bool ccd : {true,false}) for (bool reverse : {true,false})
		{
			hill(reverse,ccd);
			openSlopeSides(ccd,reverse);
		}
		valley(false); valley(true);
		for (bool ccd : {false,true}) { oneWayPlatforms(ccd); idleSliding(ccd); }
		uphillControl(); uphillRelease();
		speedAndJump(); blockers(); platformAndTrigger(); objectRemoval();
		std::cout << "Slope physics regression checks passed\n";
		return 0;
	}
	catch(const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		return 1;
	}
}
