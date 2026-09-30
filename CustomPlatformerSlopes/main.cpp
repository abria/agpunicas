#include "Window.h"
#include "SpriteFactory.h"
#include "LevelLoader.h"
#include "Audio.h"
#include "Singleton.h"
#include "Game.h"
#include "PlatformerGame.h"
#include "core_version.h"
#include "version.h"
#include <exception>

#ifdef WITH_TTF
#include "Fonts.h"
#endif

int main(int argc, char *argv[])
{
	printf("CustomPlatformerSlopes v%s\n", agp::CustomPlatformerSlopes::VERSION().c_str());
	printf("Core v%s\n\n", agp::core::VERSION().c_str());

	try
	{
		agp::Game::setInstance(new agp::PlatformerGame());
		agp::SpriteFactory::instance();
		agp::LevelLoader::instance();
		agp::Audio::instance();

#ifdef WITH_TTF
		agp::Fonts::instance();
#endif

		agp::Game::instance()->init();
		agp::Game::instance()->run();
	}
	catch (const std::exception& error)
	{
		fprintf(stderr, "ERROR: %s\n", error.what());
		return EXIT_FAILURE;
	}
	catch (const char* errMsg)
	{
		printf("ERROR: %s\n", errMsg);
		return EXIT_FAILURE;
	}
	catch (std::string errMsg)
	{
		printf("ERROR: %s\n", errMsg.c_str());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
