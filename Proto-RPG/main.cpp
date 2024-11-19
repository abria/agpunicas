#include "Window.h"
#include "SpriteFactory.h"
#include "LevelLoader.h"
#include "Audio.h"
#include "Singleton.h"
#include "Game.h"
#include "RPGGame.h"
#include "core_version.h"
#include "version.h"

int main(int argc, char *argv[])
{
    printf("Proto-RPG v%s\n", agp::RPG::VERSION().c_str());
    printf("Core v%s\n\n", agp::core::VERSION().c_str());

    try
    {
        agp::Game::setInstance(new agp::RPGGame());
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
