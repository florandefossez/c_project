#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>

#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/raycaster.hpp"
// #include "headers/entities_manager.hpp"
#include "headers/game.hpp"

int main() {
    std::cout << "Something is happening\n";
    Game game;
    std::cout << "the game has been initialized\n";
    game.run();
    return 0;
}