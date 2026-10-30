#include "Game.h"
#include "RPGGame.h"
#include "core_version.h"
#include "version.h"

int main(int argc, char *argv[])
{
    printf("ActionRPG v%s\n", agp::RPG::VERSION().c_str());
    printf("Core v%s\n\n", agp::core::VERSION().c_str());

    try
    {
        auto* game = new agp::RPGGame();
        agp::Game::setInstance(game);

        game->init();
        game->run();
    }
    catch (const char* error)
    {
        fprintf(stderr, "ERROR: %s\n", error);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ActionRPG", error, nullptr);
    }
    catch (...)
    {
        fprintf(stderr, "ERROR: unknown exception\n");
    }

    agp::Game::destroyInstance();
    SDL_Quit();
    return 0;
}
