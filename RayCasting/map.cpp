#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/hud.hpp"
#include "headers/game.hpp"

Map::Map(Game* game) : game(game) {}

void Map::load() {
    player_texture_rect.w = 10;
    player_texture_rect.h = 10;
    player_texture = IMG_LoadTexture(game->renderer, "ressources/player.png");
}


void Map::start(int level_id) {
    SDL_Surface* map_surface;
    if (sky_texture) SDL_DestroyTexture(sky_texture);

    for (unsigned int i = 0; i < 64; i++) {
        for (unsigned int j = 0; j < 64; j++) {
            map[i][j] = {false, false, 0, 0};
        }
    }

    switch (level_id) {
    case 0:
        map_surface = IMG_Load("ressources/maps/test.png");
        sky_texture = IMG_LoadTexture(game->renderer, "ressources/sky/sky1.png");
        break;
    
    case 1:
        map_surface = IMG_Load("ressources/maps/level1.png");
        sky_texture = IMG_LoadTexture(game->renderer, "ressources/sky/sky1.png");
        break;

    }

    for (unsigned int i = 0; i < 64; i++) {
        for (unsigned int j = 0; j < 64; j++) {
            Uint8* p = (Uint8*)map_surface->pixels + j * map_surface->pitch + i * 4;
            
            // type
            switch (p[0]) {
            case 0x00:
                // wall
                map[i][j].is_wall = true;
                break;
            case 0xF0:
                // door
                map[i][j].is_wall = true;
                map[i][j].is_door = true;
                break;
            case 0x40:
                // floor
                break;
            }

            // texture
            map[i][j].texture_id = p[1];
        }
    }
}

bool Map::collide(float x, float y) {
    if (x>=64 || x<0 || y>=64 || y<0) return true;
    return (map[static_cast<unsigned int>(floor(x))][static_cast<unsigned int>(floor(y))].is_wall);
}

bool Map::collide(int x, int y) {
    return (x<0 || x>=64 || y<0 || y>=64 || map[x][y].is_wall);
}

void Map::update() {
    player_texture_rect.x = static_cast<int>(game->player.position_x *  1080 / 256);
    player_texture_rect.y = static_cast<int>(game->player.position_y *  1080 / 256);
    player_sprite_angle = static_cast<double>(game->player.get_angle());
}


void Map::draw_sky() {
    int start_x = static_cast<int>(player_sprite_angle / 360.0 * 1024.0 * 3.0)%1024;
    SDL_Rect src = {start_x, 57, 0, 199};
    SDL_Rect dst = {0, 0, 0, 1080 / 3 };

    if ((start_x + 597) > 1024) {
        src.w = 1024 - start_x;
        dst.w = 1080 * src.w / 597;
        SDL_RenderCopy(game->renderer, sky_texture, &src, &dst);
        src.x = 0;
        src.w = 597 - src.w;
        dst.x = dst.w;
        dst.w = 1080 * src.w / 597;
        SDL_RenderCopy(game->renderer, sky_texture, &src, &dst);
    } else {
        src.w = 597;
        dst.w = 1080;
        SDL_RenderCopy(game->renderer, sky_texture, &src, &dst);
    }
    
}

void Map::draw() {

    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            if (collide(i, j)) {
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
                SDL_Rect wall_rect = {270 * i / 64, 270 * j / 64, 270 / 64 + 1, 270 / 64 + 1};
                SDL_RenderFillRect(game->renderer, &wall_rect);
            }
        }
    }
    
    player_texture_rect.x -= player_texture_rect.h / 2;
    player_texture_rect.y -= player_texture_rect.h / 2;

    SDL_RenderCopyEx(game->renderer, player_texture, nullptr, &player_texture_rect, player_sprite_angle, nullptr, SDL_FLIP_NONE);

    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);

    for (Entity* entity : game->entities_manager.entities) {
        SDL_Rect rect1 = {static_cast<int>(entity->position_x * 270 / 64 - 3), static_cast<int>(entity->position_y * 270 / 64 - 3), 6, 6};
        SDL_RenderFillRect(game->renderer, &rect1);
    }
}
