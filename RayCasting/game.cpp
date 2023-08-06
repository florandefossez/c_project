#include <iostream>
#include <SDL2/SDL.h>

#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/raycaster.hpp"
// #include "headers/entities_manager.hpp"
#include "headers/game.hpp"

Game::Game() : map(this), player(this), raycaster(this) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Ray casting !", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    delta_time = 1;
    running = true;
}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
    }
}

void Game::update() {
    map.update();
    player.update();
    raycaster.update();
    // entities_manager.update();

    delta_time = static_cast<float>(SDL_GetTicks() - prev_ticks) / 1000.0f;
    prev_ticks = SDL_GetTicks();

    // display FPS
    static int frame = 0;
    static float SPF = 0;
    static float animationevent = 0;

    SPF += delta_time;
    if (frame % 100 == 0) {
        SDL_SetWindowTitle(window, std::to_string(static_cast<int>(100.0 / SPF)).c_str());
        frame = 0;
        SPF = 0;
    }
    frame++;

    animation = false;
    animationevent += delta_time;
    if (animationevent > 0.2) {
        animationevent = 0;
        animation = true;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // // display 3D
    // raycaster.draw(renderer);
    // entities_manager.draw(renderer);

    // // display weapon
    // player.draw(renderer);

    // // display the map
    // map.draw(renderer);

    SDL_RenderPresent(renderer);
}
