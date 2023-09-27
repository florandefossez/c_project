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

std::array<SDL_Rect, 26> Hud::letters = {
    SDL_Rect{2, 58, 8, 7},
    SDL_Rect{11, 58, 8, 7},
    SDL_Rect{20, 58, 8, 7},
    SDL_Rect{29, 58, 8, 7},
    SDL_Rect{38, 58, 8, 7},
    SDL_Rect{47, 58, 8, 7},
    SDL_Rect{56, 58, 8, 7},
    SDL_Rect{65, 58, 8, 7},
    SDL_Rect{74, 58, 4, 7},
    SDL_Rect{79, 58, 8, 7},
    SDL_Rect{88, 58, 8, 7},
    SDL_Rect{97, 58, 8, 7},
    SDL_Rect{106, 58, 9, 7},
    SDL_Rect{116, 58, 8, 7},
    SDL_Rect{125, 58, 8, 7},
    SDL_Rect{134, 58, 8, 7},
    SDL_Rect{143, 58, 8, 8},
    SDL_Rect{152, 58, 8, 7},
    SDL_Rect{161, 58, 7, 7},
    SDL_Rect{169, 58, 8, 7},
    SDL_Rect{178, 58, 8, 7},
    SDL_Rect{187, 58, 7, 7},
    SDL_Rect{195, 58, 9, 7},
    SDL_Rect{205, 58, 9, 7},
    SDL_Rect{215, 58, 8, 7},
    SDL_Rect{224, 58, 7, 7}
};



Hud::Hud(Game* game) : game(game), menu_index(0), face(1), health(100) {}

void Hud::load(SDL_Renderer* renderer) {
    texture = IMG_LoadTexture(renderer, "ressources/hud.png");
    menu_background = IMG_LoadTexture(renderer, "ressources/menu_background.png");
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
    int width, height;
    SDL_RenderGetLogicalSize(renderer, &width, &height);
    // base layer
    static SDL_Rect src_base_layer = {0,0,319,32};
    SDL_Rect dst = {
        0,
        height - (32 * width / 319),
        width,
        32 * width / 319
    };
    SDL_RenderCopy(renderer, texture, &src_base_layer, &dst);
    
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
    dst = {
        width / 2 - (*rect_array)[face].w * width / 638,
        height - (32 * width / 638) - ((*rect_array)[face].h * width / 638),
        (*rect_array)[face].w * width / 319,
        (*rect_array)[face].h * width / 319
    };
    SDL_RenderCopy(renderer, texture, &(*rect_array)[face], &dst);

    // health
    int digit[3] = {health/100, health/10%10, health%10};
    bool b = true;
    for (int i = 0; i<3; i++) {
        if (b && !digit[i]) continue;
        b = false;
        dst = {
            width * (55+i*14) / 319,
            height - (27 * width / 319),
            width * big_numbers[digit[i]].w / 319,
            width * big_numbers[digit[i]].h / 319
        };
        SDL_RenderCopy(renderer, texture, &big_numbers[digit[i]], &dst);
    }
    
}


void Hud::draw_text(SDL_Renderer* renderer, std::string text, int x, int y, int h) {
    int i = 0;
    SDL_Rect dst = {x,y,0,h};
    SDL_Rect* src;
    while (text[i] != '\0') {
        if (text[i] == ' ') {dst.x += dst.w; i++; continue;}
        src = &letters[std::tolower(text[i]) - 'a'];
        dst.w = src->w * h / src->h;
        SDL_RenderCopy(renderer, texture, src, &dst);
        dst.x += dst.w + h / 5;
        i++;
    }   
}


void Hud::draw_menu(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, menu_background, nullptr, nullptr);
    draw_text(renderer, "new game", 150, 300, 30);
    draw_text(renderer, "option", 150, 350, 30);
    draw_text(renderer, "exit", 150, 400, 30);

    SDL_Rect dst = {110, 300 + 50*menu_index, 30, 30};
    SDL_RenderCopy(renderer, texture, &Hud::faces1_rects[7], &dst);
}

void Hud::handleEvents_menu(SDL_Event* event) {
    if (event->key.keysym.sym == SDLK_RETURN) {
        if (menu_index == 0) {
            game->start_level();
        }
        if (menu_index == 1) {
            game->state = OPTIONS;
            option_index = 0;
            return;
        }
        if (menu_index == 2) game->stop_run();
    }
    if (event->key.keysym.sym == SDLK_DOWN) menu_index = (menu_index + 1)%3;
    if (event->key.keysym.sym == SDLK_UP) menu_index = (menu_index + 2)%3;
}



void Hud::draw_option(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, menu_background, nullptr, nullptr);
    draw_text(renderer, "contoles", 260, 80, 50);

    draw_text(renderer, "move forward", 150, 200, 20);
    draw_text(renderer, "move backward", 150, 230, 20);
    draw_text(renderer, "move left", 150, 260, 20);
    draw_text(renderer, "move right", 150, 290, 20);
    draw_text(renderer, "shoot", 150, 320, 20);
    draw_text(renderer, "interact", 150, 350, 20);
    draw_text(renderer, "switch weapon", 150, 380, 20);

    draw_text(renderer, SDL_GetKeyName(game->move_forward), 650, 200, 20);
    draw_text(renderer, SDL_GetKeyName(game->move_backward), 650, 230, 20);
    draw_text(renderer, SDL_GetKeyName(game->move_left), 650, 260, 20);
    draw_text(renderer, SDL_GetKeyName(game->move_right), 650, 290, 20);
    draw_text(renderer, SDL_GetKeyName(game->shoot), 650, 320, 20);
    draw_text(renderer, SDL_GetKeyName(game->interact), 650, 350, 20);
    draw_text(renderer, SDL_GetKeyName(game->switch_weapon), 650, 380, 20);

    draw_text(renderer, "press escape to exit", 150, 500, 20);

    SDL_Rect dst = {110, 200 + 30*option_index, 20, 20};
    SDL_RenderCopy(renderer, texture, &Hud::faces1_rects[7], &dst);
}

void Hud::handleEvents_option(SDL_Event* event) {
    if (event->key.keysym.sym == SDLK_ESCAPE) {
        game->state = MENU;
        return;
    };
    if (option_index < 1) {
        game->move_forward = event->key.keysym.sym;
    } else if (option_index < 2) {
        game->move_backward = event->key.keysym.sym;
    } else if (option_index < 3) {
        game->move_left = event->key.keysym.sym;
    } else if (option_index < 4) {
        game->move_right = event->key.keysym.sym;
    } else if (option_index < 5) {
        game->shoot = event->key.keysym.sym;
    } else if (option_index < 6) {
        game->interact = event->key.keysym.sym;
    } else if (option_index < 7) {
        game->switch_weapon = event->key.keysym.sym;
    }
    option_index = (option_index + 1)%7;
}