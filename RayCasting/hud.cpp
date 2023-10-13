#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>


#include "headers/raycaster.hpp"
#include "headers/hud.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/weapon.hpp"
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


std::array<SDL_Rect, 10> Hud::grey_little_numbers = {
    SDL_Rect{176, 34, 4, 6},
    SDL_Rect{181, 34, 4, 6},
    SDL_Rect{186, 34, 4, 6},
    SDL_Rect{191, 34, 4, 6},
    SDL_Rect{196, 34, 4, 6},
    SDL_Rect{201, 34, 4, 6},
    SDL_Rect{206, 34, 4, 6},
    SDL_Rect{211, 34, 4, 6},
    SDL_Rect{216, 34, 4, 6},
    SDL_Rect{221, 34, 4, 6},
};

std::array<SDL_Rect, 10> Hud::yellow_little_numbers = {
    SDL_Rect{176, 41, 4, 6},
    SDL_Rect{181, 41, 4, 6},
    SDL_Rect{186, 41, 4, 6},
    SDL_Rect{191, 41, 4, 6},
    SDL_Rect{196, 41, 4, 6},
    SDL_Rect{201, 41, 4, 6},
    SDL_Rect{206, 41, 4, 6},
    SDL_Rect{211, 41, 4, 6},
    SDL_Rect{216, 41, 4, 6},
    SDL_Rect{221, 41, 4, 6},
};

std::array<SDL_Rect, 96> Hud::letters = {
    SDL_Rect{232, 58, 6, 7},
    SDL_Rect{87, 66, 4, 7},
    SDL_Rect{170, 66, 7, 7},
    SDL_Rect{178, 66, 7, 7},
    SDL_Rect{186, 66, 7, 8},
    SDL_Rect{194, 66, 9, 7},
    SDL_Rect{50, 66, 8, 7},
    SDL_Rect{99, 66, 4, 7},
    SDL_Rect{2, 66, 7, 7},
    SDL_Rect{10, 66, 7, 7},
    SDL_Rect{18, 66, 7, 7},
    SDL_Rect{151, 66, 7, 7},
    SDL_Rect{104, 66, 4, 7},
    SDL_Rect{92, 66, 6, 7},
    SDL_Rect{109, 66, 4, 7},
    SDL_Rect{26, 66, 7, 7},

    // 0-9
    SDL_Rect{2, 50, 8, 7},
    SDL_Rect{11, 50, 5, 7},
    SDL_Rect{17, 50, 8, 7},
    SDL_Rect{26, 50, 8, 7},
    SDL_Rect{35, 50, 7, 7},
    SDL_Rect{43, 50, 7, 7},
    SDL_Rect{51, 50, 8, 7},
    SDL_Rect{60, 50, 8, 7},
    SDL_Rect{69, 50, 8, 7},
    SDL_Rect{78, 50, 8, 7},
    SDL_Rect{114, 66, 4, 7},
    SDL_Rect{119, 66, 4, 7},
    SDL_Rect{158, 66, 5, 7},
    SDL_Rect{145, 66, 7, 7},
    SDL_Rect{164, 66, 5, 7},
    SDL_Rect{78, 66, 7, 7},

    // @
    SDL_Rect{59, 66, 9, 8},
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

    // P
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
    SDL_Rect{224, 58, 7, 7},
    SDL_Rect{134, 66, 5, 7},
    SDL_Rect{34, 66, 7, 7},
    SDL_Rect{140, 66, 5, 7},
    SDL_Rect{42, 66, 7, 7},
    SDL_Rect{69, 66, 8, 7},

    // `
    SDL_Rect{99, 66, 4, 7},
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

    // p
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
    SDL_Rect{224, 58, 7, 7},
    SDL_Rect{158, 66, 5, 7},
    SDL_Rect{129, 66, 4, 7},
    SDL_Rect{164, 66, 5, 7},
    SDL_Rect{232, 58, 6, 7},
    SDL_Rect{232, 58, 6, 7}
};



Hud::Hud(Game* game) :  menu_index(0), game(game), face(1) {}

void Hud::load(SDL_Renderer* renderer) {
    texture = IMG_LoadTexture(renderer, "ressources/hud.png");
    menu_background = IMG_LoadTexture(renderer, "ressources/menu_background.png");
}

void Hud::update(bool tick) {
    static unsigned int s;
    if (tick) s++;
    
    switch (game->player.state) {
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

}

void Hud::draw(SDL_Renderer* renderer) {
    // base layer
    static SDL_Rect src_base_layer = {0, 0, 319, 32};
    static SDL_Rect dst_base_layer = {0, 612, 1080, 108};
    SDL_RenderCopy(renderer, texture, &src_base_layer, &dst_base_layer);
    
    // marine's face
    std::array<SDL_Rect, 8>* rect_array;
    int health = static_cast<int>(game->player.health);
    if (health < 0) health = 0;
    
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
        540 - (*rect_array)[face].w * 1080 / 638,
        666 - ((*rect_array)[face].h * 1080 / 638),
        (*rect_array)[face].w * 1080 / 319,
        (*rect_array)[face].h * 1080 / 319
    };
    SDL_RenderCopy(renderer, texture, &(*rect_array)[face], &dst);

    // health
    int digit[3] = {health/100, health/10%10, health%10};
    bool b = true;
    for (int i = 0; i<3; i++) {
        if (b && !digit[i]) continue;
        b = false;
        dst = {
            1080 * (55+i*14) / 319,
            629,
            1080 * big_numbers[digit[i]].w / 319,
            1080 * big_numbers[digit[i]].h / 319
        };
        SDL_RenderCopy(renderer, texture, &big_numbers[digit[i]], &dst);
    }

    //ammo
    digit[0] = game->player.weapons[game->player.weapon]->munitions/100;
    digit[1] = game->player.weapons[game->player.weapon]->munitions/10%10;
    digit[2] = game->player.weapons[game->player.weapon]->munitions%10;
    b = true;
    for (int i = 0; i<3; i++) {
        if (b && !digit[i] && i!=2) continue;
        b = false;
        dst = {
            1080 * (3+i*14) / 319,
            629,
            1080 * big_numbers[digit[i]].w / 319,
            1080 * big_numbers[digit[i]].h / 319
        };
        SDL_RenderCopy(renderer, texture, &big_numbers[digit[i]], &dst);
    }

    //frag
    digit[0] = game->player.frag/100;
    digit[1] = game->player.frag/10%10;
    digit[2] = game->player.frag%10;
    b = true;
    for (int i = 0; i<3; i++) {
        if (b && !digit[i] && i!=2) continue;
        b = false;
        dst = {
            1080 * (98+i*14) / 319,
            629,
            1080 * big_numbers[digit[i]].w / 319,
            1080 * big_numbers[digit[i]].h / 319
        };
        SDL_RenderCopy(renderer, texture, &big_numbers[digit[i]], &dst);
    }

    //armor
    int armor = static_cast<int>(game->player.armor);
    digit[0] = armor/100;
    digit[1] = armor/10%10;
    digit[2] = armor%10;
    b = true;
    for (int i = 0; i<3; i++) {
        if (b && !digit[i] && i!=2) continue;
        b = false;
        dst = {
            47*i + 612,
            629,
            1080 * big_numbers[digit[i]].w / 319,
            1080 * big_numbers[digit[i]].h / 319
        };
        SDL_RenderCopy(renderer, texture, &big_numbers[digit[i]], &dst);
    }

    //weapon numbers
    for (int line=0; line<2; line++) {
        for (int row=0; row<3; row++) {
            dst = {
                1080 * (12*row + 242) / 319,
                625 + 33*line,
                13,
                20
            };
            SDL_RenderCopy(
                renderer,
                texture,
                game->player.weapons[3*line+row]->available ? &yellow_little_numbers[3*line+row+1] : &grey_little_numbers[3*line+row+1],
                &dst
            );
        }
    }

    // items
    SDL_Rect src = {227,34,10,7};
    dst = {944, 619, 30, 26};
    for (int i=0; i<3; i++) {
        if (game->player.armors[i]) {
            SDL_RenderCopy(game->renderer, texture, &src, &dst);
        }
        dst.x += 47;
        src.x += 11;
    }

    // game over
    if (game->player.game_over)
        draw_text(game->renderer, "Game Over", 100, 300, 70);
}


void Hud::draw_text(SDL_Renderer* renderer, std::string text, int x, int y, int h) {
    int i = 0;
    SDL_Rect dst = {x,y,0,h};
    SDL_Rect* src;
    while (text[i] != '\0') {
        src = &letters[text[i] - ' '];
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

    switch (event->key.keysym.sym) {
    case SDLK_RETURN:
        switch (menu_index) {
        case 0:
            game->state = LEVEL_MENU;
            break;
        case 1:
            game->state = OPTIONS;
            menu_index = 0;
            break;
        case 2:
            game->stop_run();
            break;
        }
        break;
    
    case SDLK_DOWN:
        menu_index = (menu_index + 1)%3;
        break;
    
    case SDLK_UP:
        menu_index = (menu_index + 2)%3;
        break;

    }
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

    SDL_Rect dst = {110, 200 + 30*menu_index, 20, 20};
    SDL_RenderCopy(renderer, texture, &Hud::faces1_rects[7], &dst);
}

void Hud::handleEvents_option(SDL_Event* event) {
    if (event->key.keysym.sym == SDLK_ESCAPE) {
        game->state = MENU;
        menu_index = 0;
        return;
    };
    switch (menu_index) {
    case 0:
        game->move_forward = event->key.keysym.sym;
        break;
    case 1:
        game->move_backward = event->key.keysym.sym;
        break;
    case 2:
        game->move_left = event->key.keysym.sym;
        break;
    case 3:
        game->move_right = event->key.keysym.sym;
        break;
    case 4:
        game->shoot = event->key.keysym.sym;
        break;
    case 5:
        game->interact = event->key.keysym.sym;
        break;
    case 6:
        game->switch_weapon = event->key.keysym.sym;  
        break;
    }
    menu_index = (menu_index + 1)%7;
}


void Hud::draw_level_menu() {
    SDL_RenderCopy(game->renderer, menu_background, nullptr, nullptr);
    draw_text(game->renderer, "Test area", 150, 300, 30);
    draw_text(game->renderer, "Level 1", 150, 350, 30);
    draw_text(game->renderer, "exit", 150, 400, 30);

    SDL_Rect dst = {110, 300 + 50*menu_index, 30, 30};
    SDL_RenderCopy(game->renderer, texture, &Hud::faces1_rects[7], &dst);
}

void Hud::handleEvents_level_menu(SDL_Event* event) {
    switch (event->key.keysym.sym) {
    case SDLK_RETURN:
        switch (menu_index) {
        case 0:
            game->state = PLAY;
            game->start_level(0);
            break;
        case 1:
            game->state = PLAY;
            game->start_level(1);
            break;
        case 2:
            game->state = MENU;
            break;
        }
        break;
    
    case SDLK_DOWN:
        menu_index = (menu_index + 1)%3;
        break;
    
    case SDLK_UP:
        menu_index = (menu_index + 2)%3;
        break;
    }
}
