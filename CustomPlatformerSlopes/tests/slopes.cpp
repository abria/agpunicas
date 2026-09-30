// Regression checks for the port of SimplePlatformer-Slopes to SMB3/core.
#include "DynamicObject.h"
#include "Slope.h"
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
	void gravity(float value) { _yGravityForce = value; }
	void defaultForces(float maxSpeed = 6) { defaultPhysics(); _xVelMax = maxSpeed; }
	bool collision(CollidableObject* with, bool begin, Direction) override
	{
		if (!begin) exits[with]++;
		else contacts[with]++;
		return true;
	}
};

void hill(bool reverse, bool ccd)
{
	Scene scene({-20,-20,60,50},{16,16});
	new StaticObject(&scene, {-10,0,40,3}, nullptr);
	auto* up = new Slope(&scene, {0,-2,4,2}, {190,120,65}, true);
	new StaticObject(&scene, {4,-2,2,2}, nullptr);
	auto* down = new Slope(&scene, {6,-2,4,2}, {190,120,65}, false);
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
	new StaticObject(&scene, {-10,0,40,3}, nullptr);
	auto* left = new Slope(&scene, {0,-3,4,3}, {190,120,65}, false);
	auto* right = new Slope(&scene, {4,-3,4,3}, {190,120,65}, true);
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
	auto* slope = new Slope(&scene, {0,-4,8,4}, {190,120,65}, true);
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
	auto* slope = new Slope(&scene, {0,-4,4,4}, {190,120,65}, true);
	auto* body = new Body(&scene, {1,-2.6f,0.6f,1});
	new StaticObject(&scene, {2,-6,0.5f,5}, nullptr);
	scene.update(0);
	body->velocity(3);
	body->move(Direction::RIGHT);
	for (int i=0;i<100;++i) body->update(0.01f);
	require(body->sceneCollider().right() <= 2.001f, "passed through wall on slope");
	new StaticObject(&scene, {0,-3,1.8f,0.2f}, nullptr);
	scene.update(0);
	body->setPos({1,-2.6f});
	body->velocity(3);
	body->move(Direction::RIGHT);
	for (int i=0;i<100;++i) body->update(0.01f);
	require(body->sceneCollider().top() >= -2.801f, "climbed through ceiling");
	Vec2Df point, normal;
	float time;
	require(slope->sweep({4.2f,-1,0.6f,1},{-1,0},point,normal,time) && normal.x > 0, "high side is not solid");
	require(slope->sweep({1,1,0.6f,1},{0,-2},point,normal,time) && normal.y > 0, "underside is not solid");
}

void platformAndTrigger()
{
	Scene scene({-20,-20,60,50},{16,16});
	new Slope(&scene, {0,-2,4,2}, {190,120,65}, true);
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
		auto* slope = new Slope(&scene, {0,-20*gradient,20,20*gradient}, {190,120,65}, risesRight);
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
	auto* left = new Slope(&valley, {0,-3,4,3}, {190,120,65}, false);
	new Slope(&valley, {4,-3,4,3}, {190,120,65}, true);
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
		auto* slope = new Slope(&scene, {0,-20*gradient,20,20*gradient}, {190,120,65}, true);
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
		auto* slope = new Slope(&scene, {0,-20*gradient,20,20*gradient}, {190,120,65}, risesRight);
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
		if (object->to<Slope*>()) ramps++;
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
	auto categories = level.json().at("categories").get<std::vector<std::string>>();
	auto saved = level.json();
	saved["objects"] = nlohmann::ordered_json::array();
	for (const auto& object : level.objects())
	{
		auto* editable = new EditableObject(&editorObjects, object, categories);
		auto serialized = editable->toJson();
		for (const char* key : {"risesRight", "content", "color", "layer"})
			if (object.contains(key)) require(serialized.at(key) == object.at(key), std::string("editor lost ") + key);
		saved["objects"].push_back(serialized);
	}
	std::string savedPath = std::string(SDL_GetBasePath()) + "levels/roundtrip-test.json";
	LevelData::save(savedPath, saved);
	LevelData roundtrip(savedPath);
	require(roundtrip.json().at("scene") == level.json().at("scene"), "scene settings changed in roundtrip");
	delete LevelLoader::instance()->load("roundtrip-test");
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
		for (bool ccd : {true,false}) for (bool reverse : {true,false}) hill(reverse,ccd);
		valley(false); valley(true);
		for (bool ccd : {false,true}) { oneWayPlatforms(ccd); idleSliding(ccd); }
		uphillControl(); uphillRelease();
		speedAndJump(); blockers(); platformAndTrigger();
		std::cout << "Slope physics regression checks passed\n";
		return 0;
	}
	catch(const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		return 1;
	}
}
