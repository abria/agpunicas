#include "Game.h"
#include "PlatformerGame.h"
#include "core_version.h"
#include "version.h"

int main(int argc, char *argv[])
{
    printf("CustomPlatformerSlopes v%s\n", agp::CustomPlatformerSlopes::VERSION().c_str());
    printf("Core v%s\n\n", agp::core::VERSION().c_str());

    try
    {
        auto* game = new agp::PlatformerGame();
        agp::Game::setInstance(game);

        game->init();
        game->run();
    }
    catch (const char* error)
    {
        fprintf(stderr, "ERROR: %s\n", error);
    }
    catch (...)
    {
        fprintf(stderr, "ERROR: unknown exception\n");
    }

    agp::Game::destroyInstance();
    SDL_Quit();
    return 0;
}
