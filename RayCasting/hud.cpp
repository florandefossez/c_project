#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>


#include "headers/raycaster.hpp"
#include "headers/hud.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"

std::array<SDL_Rect, 8> Hud::faces1_rects = {
    SDL_Rect{0, 109, 24, 29},
    SDL_Rect{26, 109, 24, 29},
    SDL_Rect{52, 109, 24, 29},
    SDL_Rect{0, 269, 24, 30},
    SDL_Rect{0, 301, 26, 30},
    SDL_Rect{0, 334, 26, 30},
    SDL_Rect{0, 366, 24, 31},
    SDL_Rect{0, 399, 24, 30}
};


Hud::Hud(SDL_Renderer* renderer) : face(1) {
    SDL_Surface* surface = IMG_Load("ressources/hud.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void Hud::update(bool tick) {
    static unsigned int s;
    if (tick) s++;
    face = (s/20)%8;
}

void Hud::draw(SDL_Renderer* renderer) {
    static SDL_Rect src = {0,0,319,32};
    static SDL_Rect dst = {0,WINDOW_HEIGHT - 32 * WINDOW_WIDTH / 319, WINDOW_WIDTH, 32 * WINDOW_WIDTH / 319};
    SDL_RenderCopy(renderer, texture, &src, &dst);
    
    SDL_Rect dst_face = {
        WINDOW_WIDTH / 2 - faces1_rects[face].w * WINDOW_WIDTH / 319 / 2,
        WINDOW_HEIGHT - (32 * WINDOW_WIDTH / 319) + (32 * WINDOW_WIDTH / 319)/2 - (faces1_rects[face].h * WINDOW_WIDTH / 319) / 2,
        faces1_rects[face].w * WINDOW_WIDTH / 319,
        faces1_rects[face].h * WINDOW_WIDTH / 319
    };
    SDL_RenderCopy(renderer, texture, &faces1_rects[face], &dst_face);
}