#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif
#include <unistd.h>

#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/raycaster.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"

Game::Game() : player(this), map(this), raycaster(this), entities_manager(this) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Ray casting !", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    scene = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetTextureBlendMode(scene, SDL_BLENDMODE_BLEND);

    delta_time = 1;
    running = true;

    map.load();
    raycaster.load();
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

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
        else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
#ifdef __EMSCRIPTEN__
                SDL_ShowCursor(SDL_ENABLE);
#else
                running = false;
#endif
            }
            if (event.key.keysym.sym == SDLK_SPACE) {
                player.shoot();
            }
        }
    }
}

void Game::update() {
    map.update();
    player.update();
    raycaster.update();
    entities_manager.update();

    // std::cout << entities_manager.entities.end() - entities_manager.entities.begin() << std::endl;
    // for (Entity* entity : entities_manager.entities) {
    //     std::cout << entity->position_x << " " << entity->position_y << " " << entity->size << std::endl;
    // }
    // std::cout << std::endl;

    delta_time = static_cast<float>(SDL_GetTicks() - prev_ticks) / 1000.0f;
    prev_ticks = SDL_GetTicks();

    if (delta_time > 1.f) delta_time = 1.f;

    // display FPS
    // static int frame = 0;
    // static float SPF = 0;
    static float animationevent = 0;

    // SPF += delta_time;
    // if (frame % 100 == 0) {
    //     SDL_SetWindowTitle(window, std::to_string(static_cast<int>(100.0 / SPF)).c_str());
    //     frame = 0;
    //     SPF = 0;
    // }
    // frame++;

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
    scene_pixels.fill(255);

    // // display 3D
    raycaster.draw();
    entities_manager.draw();

    // // display weapon
    // player.draw(renderer);

    SDL_UpdateTexture(scene, nullptr, reinterpret_cast<void *>(scene_pixels.data()), WINDOW_WIDTH*4);
    SDL_RenderCopy(renderer, scene, NULL, NULL);

    // // display the map
    map.draw();

    SDL_RenderPresent(renderer);
}
