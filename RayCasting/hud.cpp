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

std::array<SDL_Rect, 8> Hud::faces2_rects = {
    SDL_Rect{0, 140, 24, 29},
    SDL_Rect{26, 140, 24, 29},
    SDL_Rect{52, 140, 24, 29},
    SDL_Rect{26, 269, 24, 30},
    SDL_Rect{28, 301, 26, 30},
    SDL_Rect{28, 334, 26, 30},
    SDL_Rect{26, 366, 24, 31},
    SDL_Rect{26, 399, 24, 30}
};

std::array<SDL_Rect, 8> Hud::faces3_rects = {
    SDL_Rect{0, 171, 24, 29},
    SDL_Rect{26, 171, 24, 29},
    SDL_Rect{52, 171, 24, 29},
    SDL_Rect{52, 269, 24, 30},
    SDL_Rect{56, 301, 27, 30},
    SDL_Rect{56, 334, 27, 30},
    SDL_Rect{52, 366, 24, 31},
    SDL_Rect{52, 399, 24, 30}
};

std::array<SDL_Rect, 8> Hud::faces4_rects = {
    SDL_Rect{0, 203, 24, 29},
    SDL_Rect{26, 203, 24, 29},
    SDL_Rect{52, 203, 24, 29},
    SDL_Rect{78, 269, 24, 30},
    SDL_Rect{85, 301, 29, 31},
    SDL_Rect{85, 334, 28, 30},
    SDL_Rect{78, 366, 24, 31},
    SDL_Rect{78, 399, 24, 30}
};

std::array<SDL_Rect, 8> Hud::faces5_rects = {
    SDL_Rect{0, 236, 24, 29},
    SDL_Rect{26, 236, 24, 29},
    SDL_Rect{52, 236, 24, 29},
    SDL_Rect{104, 269, 24, 30},
    SDL_Rect{116, 301, 30, 31},
    SDL_Rect{115, 334, 28, 30},
    SDL_Rect{104, 366, 24, 31},
    SDL_Rect{104, 399, 24, 30}
};


std::array<SDL_Rect, 10> Hud::big_numbers = {
    SDL_Rect{2, 33, 14, 16},
    SDL_Rect{17, 33, 11, 16},
    SDL_Rect{29, 33, 14, 16},
    SDL_Rect{44, 33, 14, 16},
    SDL_Rect{59, 33, 14, 16},
    SDL_Rect{74, 33, 14, 16},
    SDL_Rect{89, 33, 14, 16},
    SDL_Rect{104, 33, 14, 16},
    SDL_Rect{119, 33, 14, 16},
    SDL_Rect{134, 33, 14, 16}
};


Hud::Hud(SDL_Renderer* renderer) : face(1), health(100) {
    SDL_Surface* surface = IMG_Load("ressources/hud.png");
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void Hud::update(bool tick, Player* player) {
    static unsigned int s;
    if (tick) s++;
    
    switch (player->state) {
    case player_state_t::CALM:
        face = (s/20)%3;
        break;
    case player_state_t::CLENCH:
        face = (s/20)%3 + 3;
        break;
    case player_state_t::SCREAM:
        face = 6;
        break;
    case player_state_t::LAUGHT:
        face = 7;
        break;
    default:
        face = 0;
        break;
    }

    health = static_cast<int>(player->health);
    if (health < 0) health = 0;
}

void Hud::draw(SDL_Renderer* renderer) {
    // base layer
    static SDL_Rect src_base_layer = {0,0,319,32};
    static SDL_Rect dst_base_layer = {
        0,
        WINDOW_HEIGHT - (32 * WINDOW_WIDTH / 319),
        WINDOW_WIDTH,
        32 * WINDOW_WIDTH / 319
    };
    SDL_RenderCopy(renderer, texture, &src_base_layer, &dst_base_layer);
    
    // marine's face
    std::array<SDL_Rect, 8>* rect_array;
    
    if (health > 79) {
        rect_array = &faces1_rects;
    } else if (health > 59) {
        rect_array = &faces2_rects;
    } else if (health > 39) {
        rect_array = &faces3_rects;
    } else if (health > 19) {
        rect_array = &faces4_rects;
    } else {
        rect_array = &faces5_rects;
    }
    SDL_Rect dst = {
        WINDOW_WIDTH / 2 - (*rect_array)[face].w * WINDOW_WIDTH / 638,
        WINDOW_HEIGHT - (32 * WINDOW_WIDTH / 638) - ((*rect_array)[face].h * WINDOW_WIDTH / 638),
        (*rect_array)[face].w * WINDOW_WIDTH / 319,
        (*rect_array)[face].h * WINDOW_WIDTH / 319
    };
    SDL_RenderCopy(renderer, texture, &(*rect_array)[face], &dst);

    // health
    int digit[3] = {health/100, health/10%10, health%10};
    bool b = true;
    for (int i = 0; i<3; i++) {
        if (b && !digit[i]) continue;
        b = false;
        dst = {
            WINDOW_WIDTH * (55+i*14) / 319,
            WINDOW_HEIGHT - (27 * WINDOW_WIDTH / 319),
            WINDOW_WIDTH * big_numbers[digit[i]].w / 319,
            WINDOW_WIDTH * big_numbers[digit[i]].h / 319
        };
        SDL_RenderCopy(renderer, texture, &big_numbers[digit[i]], &dst);
    }
    
}