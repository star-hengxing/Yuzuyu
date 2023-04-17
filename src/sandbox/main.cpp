#include "Game.hpp"

auto main(int argc, char** argv) -> int
{
    auto game = Game::create(argc, argv);
    game->initialize();
    game->run();
    game->clean();
}
