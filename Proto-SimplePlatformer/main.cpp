#include "Window.h"
#include "SpriteFactory.h"
#include "LevelLoader.h"
#include "Audio.h"
#include "Singleton.h"
#include "Game.h"
#include "PlatformerGame.h"
#include "core_version.h"
#include "version.h"

int main(int argc, char *argv[])
{
	printf("Proto-SimplePlatformer v%s\n", agp::SimplePlatformer::VERSION().c_str());
	printf("Core v%s\n\n", agp::core::VERSION().c_str());

	try
	{
		agp::Game::setInstance(new agp::PlatformerGame());
		agp::SpriteFactory::instance();
		agp::LevelLoader::instance();
		agp::Audio::instance();

		agp::Game::instance()->init();
		agp::Game::instance()->run();
	}
	catch (const char* errMsg)
	{
		printf("ERROR: %s\n", errMsg);
	}
	catch (std::string errMsg)
	{
		printf("ERROR: %s\n", errMsg.c_str());
	}

	return EXIT_SUCCESS;
}
