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
#include "headers/hud.hpp"
#include "headers/weapon.hpp"
#include "headers/game.hpp"

Game::Game() : width(1024), state(MENU), map(this), player(this), raycaster(this), entities_manager(this), hud(this) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 16384);

    game_music = Mix_LoadMUS("ressources/sounds/game_music.wav");
    menu_music = Mix_LoadMUS("ressources/sounds/menu_music.wav");

    window = SDL_CreateWindow("Ray casting !", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    running = true;

    scene_pixels = new Uint32[width * (2 * width / 3)];

    scene = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, (2 * width / 3));
    SDL_SetTextureBlendMode(scene, SDL_BLENDMODE_BLEND);

    delta_time = 1;
    prev_ticks = SDL_GetTicks();

    map.load();
    raycaster.load();
    player.load();
    hud.load(renderer);
    
    move_forward = SDLK_z;
    move_backward = SDLK_s;
    move_right = SDLK_d;
    move_left = SDLK_q;
    shoot = SDLK_SPACE;
    interact = SDLK_u;
    switch_weapon = SDLK_k;

    Mix_PlayMusic(menu_music, -1);
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
    SDL_DestroyTexture(scene);
    scene = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, (2 * width / 3));
    SDL_SetTextureBlendMode(scene, SDL_BLENDMODE_BLEND);
    raycaster.update_width();
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            return;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p) toggleFullscreen();

        switch (state) {
        case MENU:
            if (event.type != SDL_KEYDOWN) continue;
            hud.handleEvents_menu(&event);
            break;
        
        case OPTIONS:
            if (event.type != SDL_KEYDOWN) continue;
            hud.handleEvents_option(&event);
            break;
        
        case LEVEL_MENU:
            if (event.type != SDL_KEYDOWN) continue;
            hud.handleEvents_level_menu(&event);
            break;
        
        case PLAY:
#ifdef __EMSCRIPTEN__
            if (event.type == SDL_MOUSEMOTION) {
                player.rotate((float) event.motion.xrel * 0.0015f);
            }
#endif
            if (event.type != SDL_KEYDOWN) continue;
            if (event.key.keysym.sym == switch_weapon) player.switch_weapon();
            if (event.key.keysym.sym == interact) raycaster.trigger();
            if (event.key.keysym.sym == SDLK_ESCAPE) menu();
            if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_6 && player.weapons[event.key.keysym.sym - SDLK_1]->available) player.weapon = event.key.keysym.sym - SDLK_1;
            break;
        }

    }
}

void Game::update() {
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

    switch (state) {    
    case PLAY:
        map.update();
        player.update();
        hud.update(animation);
        raycaster.update();
        entities_manager.update();
        break;
    
    default:
        break;
    }
}

void Game::render() {
    // padding colors
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
    SDL_RenderClear(renderer);

    switch (state) {
    case MENU:
        hud.draw_menu(renderer);
        break;
    
    case OPTIONS:
        hud.draw_option(renderer);
        break;
    
    case LEVEL_MENU:
        hud.draw_level_menu();
        break;
    
    case PLAY:
        std::fill(scene_pixels, scene_pixels + (width * (2*width/3)), 0);
        SDL_SetRenderDrawColor(renderer, raycaster.fog_color[3], raycaster.fog_color[2], raycaster.fog_color[1], 255);
        SDL_RenderFillRect(renderer, nullptr);

        map.draw_sky();
        raycaster.draw();
        entities_manager.draw();

        SDL_UpdateTexture(scene, nullptr, (void *) scene_pixels, width*4);
        SDL_RenderCopy(renderer, scene, nullptr, nullptr);

        player.draw();
        hud.draw(renderer);
        // map.draw();
        break;
    }

    SDL_RenderPresent(renderer);
}


void Game::start_level(int level_id) {
    Mix_PlayMusic(game_music, -1);
    state = PLAY;
    map.start(level_id);
    player.start(level_id);
    entities_manager.start(level_id);
    raycaster.start();
}

void Game::menu() {
    Mix_PlayMusic(menu_music, -1);
    Mix_HaltChannel(1);
    state = MENU;
    hud.menu_index = 0;
};

void Game::stop_run() {
    running = false;
}