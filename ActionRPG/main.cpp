#include "Window.h"
#include "SpriteFactory.h"
#include "Audio.h"
#include "HUD.h"
#include "Game.h"
#include "RPGGame.h"
#include "core_version.h"
#include "version.h"

#ifdef WITH_TTF
#include "Fonts.h"
#endif

int main(int argc, char *argv[])
{
    printf("ActionRPG v%s\n", agp::RPG::VERSION().c_str());
    printf("Core v%s\n\n", agp::core::VERSION().c_str());

    agp::RPGGame* game = nullptr;
    int exitCode = EXIT_SUCCESS;

    try
    {
        game = new agp::RPGGame();
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
    catch (const char* error)
    {
        fprintf(stderr, "ERROR: %s\n", error);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ActionRPG", error, nullptr);
        exitCode = EXIT_FAILURE;
    }
    catch (...)
    {
        fprintf(stderr, "ERROR: unknown exception\n");
        exitCode = EXIT_FAILURE;
    }

    // Scenes first, then shared resources, then the window and SDL.
    if (game)
        game->clearScenes();
    agp::HUD::destroyInstance();
    agp::SpriteFactory::destroyInstance();
    agp::Audio::destroyInstance();
#ifdef WITH_TTF
    agp::Fonts::destroyInstance();
#endif
    agp::Game::destroyInstance();
    SDL_Quit();
    return exitCode;
}
