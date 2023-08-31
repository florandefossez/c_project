#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"

Map::Map(Game* game) : game(game) {
    player_texture_rect.w = 2 * MINIMAP / 32;
    player_texture_rect.h = 2 * MINIMAP / 32;
}

void Map::load() {
    player_texture = IMG_LoadTexture(game->renderer, "ressources/player.png");
    SDL_Surface* map_surface = IMG_Load( "ressources/map.png");

    for (unsigned int i = 0; i < 32; i++) {
        for (unsigned int j = 0; j < 32; j++) {
            Uint8* p = (Uint8*)map_surface->pixels + j * map_surface->pitch + i * 4;
            if (p[3] == 255) {
                this->map[i][j].is_wall = true;
            } else {
                this->map[i][j].is_wall = false;
            }
            map[i][j].dir = 0;
        }
    }
}

bool Map::collide(float x, float y) {
    if (x>=32 || x<0 || y>=32 || y<0) return true;
    return (map[static_cast<unsigned int>(floor(x))][static_cast<unsigned int>(floor(y))].is_wall);
}

bool Map::collide(int x, int y) {
    return (map[x][y].is_wall);
}

void Map::update() {
    player_texture_rect.x = static_cast<int>(game->player.position_x * MINIMAP / 32.0);
    player_texture_rect.y = static_cast<int>(game->player.position_y * MINIMAP / 32.0);
    player_sprite_angle = static_cast<double>(game->player.get_angle());
}

void Map::draw() {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            // SDL_SetRenderDrawColor(game->renderer, 0, 60*map[i][j].dir, 0, 255);
            // SDL_Rect wall_rect = {MINIMAP * i / 32, MINIMAP * j / 32, MINIMAP / 32 + 1, MINIMAP / 32 + 1};
            // SDL_RenderFillRect(game->renderer, &wall_rect);

            if (collide(i, j)) {
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
                SDL_Rect wall_rect = {MINIMAP * i / 32, MINIMAP * j / 32, MINIMAP / 32 + 1, MINIMAP / 32 + 1};
                SDL_RenderFillRect(game->renderer, &wall_rect);
            }
        }
    }
    
    SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
    for(int r=0; r<WINDOW_WIDTH; r+=10) {
        SDL_RenderDrawLine(game->renderer, vision_field_points[r].x, vision_field_points[r].y, player_texture_rect.x, player_texture_rect.y);
    }
    player_texture_rect.x -= player_texture_rect.h / 2;
    player_texture_rect.y -= player_texture_rect.h / 2;

    SDL_RenderCopyEx(game->renderer, player_texture, nullptr, &player_texture_rect, player_sprite_angle, nullptr, SDL_FLIP_NONE);

    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);

    for (Entity* entity : game->entities_manager.entities) {
        SDL_Rect rect1 = {static_cast<int>(entity->position_x * MINIMAP / 32 - 3), static_cast<int>(entity->position_y * MINIMAP / 32 - 3), 6, 6};
        SDL_RenderFillRect(game->renderer, &rect1);
    }
}
