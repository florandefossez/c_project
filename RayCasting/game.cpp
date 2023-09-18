#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif
#include <unistd.h>

#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/raycaster.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"

Game::Game() : width(1024), map(this), player(this), raycaster(this), entities_manager(this) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Ray casting !", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    scene_pixels = new Uint32[width * (2 * width / 3)];

    SDL_RenderSetLogicalSize(renderer, width, 2 * width / 3);
    scene = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, (2 * width / 3));
    SDL_SetTextureBlendMode(scene, SDL_BLENDMODE_BLEND);

    delta_time = 1;
    prev_ticks = SDL_GetTicks();
    running = true;

    map.load();
    raycaster.load();
    player.load();
}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void loop_handler(void *arg) {
    Game *game = (Game*) arg;

    game->handleEvents();
    game->update();
    game->render();

}

void Game::run() {
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(loop_handler, this, -1, 1);
#else
    while (running) {
        handleEvents();
        update();
        render();
    }
#endif
}

void Game::toggleFullscreen() {
    bool IsFullscreen = SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
    SDL_SetWindowFullscreen(window, IsFullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
}

void Game::update_width(int w) {
    this->width = w;
    free(scene_pixels);
    scene_pixels = new Uint32[width * (2 * width / 3)];
    SDL_RenderSetLogicalSize(renderer, width, 2 * width / 3);
    SDL_DestroyTexture(scene);
    scene = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, width, (2 * width / 3));
    SDL_SetTextureBlendMode(scene, SDL_BLENDMODE_BLEND);
    raycaster.update_width();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
#ifdef __EMSCRIPTEN__
#else
                running = false;
#endif
            }
            if (event.key.keysym.sym == SDLK_p) {
                toggleFullscreen();                
            }
            if (event.key.keysym.sym == SDLK_SPACE) {
                player.shoot();
            }
            if (event.key.keysym.sym == SDLK_u) {
                raycaster.trigger();
            }
        }
    }
}

void Game::update() {
    map.update();
    player.update();
    raycaster.update();
    entities_manager.update();

    delta_time = static_cast<float>(SDL_GetTicks() - prev_ticks) / 1000.0f;
    prev_ticks = SDL_GetTicks();

    if (delta_time > 1.f) delta_time = 1.f;

    static float animationevent = 0;

    animation = false;
    animationevent += delta_time;
    if (animationevent > 0.05) {
        animationevent = 0;
        animation = true;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);
    std::fill(scene_pixels, scene_pixels + (width * (2*width/3)), 0);

    map.draw_sky();
    // // display 3D
    raycaster.draw();
    entities_manager.draw();

    SDL_UpdateTexture(scene, nullptr, (void *) scene_pixels, width*4);
    SDL_RenderCopy(renderer, scene, NULL, NULL);

    player.draw();
    map.draw();

    SDL_RenderPresent(renderer);
}
