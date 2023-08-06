#include <SDL2/SDL.h>

#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/raycaster.hpp"
// #include "headers/entities_manager.hpp"
#include "headers/game.hpp"

int main() {
    Game game;
    game.run();
    return 0;
}