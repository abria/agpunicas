#include "Window.h"
#include "SpriteFactory.h"
#include "LevelLoader.h"
#include "Audio.h"
#include "Singleton.h"
#include "Game.h"
#include "ComplexPlatformerGame.h"
#include "core_version.h"
#include "version.h"
#include <exception>

#ifdef WITH_TTF
#include "Fonts.h"
#endif

int main(int argc, char *argv[])
{
    printf("Box2DPlatformer v%s\n", agp::ComplexPlatformer::VERSION().c_str());
    printf("Core v%s\n\n", agp::core::VERSION().c_str());

	try
	{
		agp::Game::setInstance(new agp::ComplexPlatformerGame());
		agp::SpriteFactory::instance();
		agp::LevelLoader::instance();
		agp::Audio::instance();

#ifdef WITH_TTF
        agp::Fonts::instance();
#endif

		agp::Game::instance()->init();
		agp::Game::instance()->run();
	}
    catch (const std::exception& err)
    {
        printf("ERROR: %s\n", err.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Box2DPlatformer", err.what(), nullptr);
    }
    catch (const char* errMsg)
    {
        printf("ERROR: %s\n", errMsg);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Box2DPlatformer", errMsg, nullptr);
    }
    catch (std::string errMsg)
    {
        printf("ERROR: %s\n", errMsg.c_str());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Box2DPlatformer", errMsg.c_str(), nullptr);
    }

    return EXIT_SUCCESS;
}
