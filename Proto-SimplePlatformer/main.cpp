#include "Window.h"
#include "SpriteFactory.h"
#include "LevelLoader.h"
#include "Audio.h"
#include "Game.h"

int main(int argc, char *argv[])
{
    agp::Game::instance();
    agp::SpriteFactory::instance();
    agp::LevelLoader::instance();
    agp::Audio::instance();

    agp::Game::instance()->run();

    return EXIT_SUCCESS;
}
