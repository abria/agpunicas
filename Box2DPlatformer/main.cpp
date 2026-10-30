#include "Window.h"
#include "SpriteFactory.h"
#include "Audio.h"
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
    printf("Box2DPlatformer v%s\n", agp::Box2DPlatformer::VERSION().c_str());
    printf("Core v%s\n\n", agp::core::VERSION().c_str());

    agp::PlatformerGame* game = nullptr;
    int exitCode = EXIT_SUCCESS;

    try
    {
        game = new agp::PlatformerGame();
        agp::Game::setInstance(game);

        // Initialize shared services before loading the game.
        agp::SpriteFactory::instance();
        agp::Audio::instance();

#ifdef WITH_TTF
        agp::Fonts::instance();
#endif

        game->init();
        game->run();
    }
    catch (const std::exception& err)
    {
        exitCode = EXIT_FAILURE;
        printf("ERROR: %s\n", err.what());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Box2DPlatformer", err.what(), nullptr);
    }
    catch (const char* errMsg)
    {
        exitCode = EXIT_FAILURE;
        printf("ERROR: %s\n", errMsg);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Box2DPlatformer", errMsg, nullptr);
    }
    catch (std::string errMsg)
    {
        exitCode = EXIT_FAILURE;
        printf("ERROR: %s\n", errMsg.c_str());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Box2DPlatformer", errMsg.c_str(), nullptr);
    }
    catch (...)
    {
        fprintf(stderr, "ERROR: unknown exception\n");
        exitCode = EXIT_FAILURE;
    }

    // Scenes first, then shared resources, then the window and SDL.
    if (game)
        game->clearScenes();
    agp::SpriteFactory::destroyInstance();
    agp::Audio::destroyInstance();
#ifdef WITH_TTF
    agp::Fonts::destroyInstance();
#endif
    agp::Game::destroyInstance();
    SDL_Quit();
    return exitCode;
}
