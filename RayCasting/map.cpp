#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"

Map::Map(Game* game) : game(game) {
    player_texture_rect.w = 2 * game->width / MAP_WIDTH / 4;
    player_texture_rect.h = 2 * game->width / MAP_WIDTH / 4;
}

void Map::load() {
    player_texture = IMG_LoadTexture(game->renderer, "ressources/player.png");
    SDL_Surface* map_surface = IMG_Load( "ressources/map.png");

    for (unsigned int i = 0; i < MAP_WIDTH; i++) {
        for (unsigned int j = 0; j < MAP_WIDTH; j++) {
            Uint8* p = (Uint8*)map_surface->pixels + j * map_surface->pitch + i * 4;
            if (p[3] == 255) {
                this->map[i][j].is_wall = true;
            } else {
                this->map[i][j].is_wall = false;
            }
            map[i][j].dir = 0;
            map[i][j].is_door = false;
        }
    }
    map[8][41].is_door = true;
    map[8][41].is_wall = true;
    map[4][57].is_wall = true;
    map[4][57].is_door = true;
}

bool Map::collide(float x, float y) {
    if (x>=MAP_WIDTH || x<0 || y>=MAP_WIDTH || y<0) return true;
    return (map[static_cast<unsigned int>(floor(x))][static_cast<unsigned int>(floor(y))].is_wall);
}

bool Map::collide(int x, int y) {
    return (map[x][y].is_wall);
}

void Map::update() {
    player_texture_rect.w = 2 * game->width / MAP_WIDTH / 4;
    player_texture_rect.h = 2 * game->width / MAP_WIDTH / 4;
    player_texture_rect.x = static_cast<int>(game->player.position_x *  game->width/4 / MAP_WIDTH);
    player_texture_rect.y = static_cast<int>(game->player.position_y *  game->width/4 / MAP_WIDTH);
    player_sprite_angle = static_cast<double>(game->player.get_angle());
}

void Map::draw() {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

    for (int i = 0; i < MAP_WIDTH; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (collide(i, j)) {
                SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
                SDL_Rect wall_rect = {game->width/4 * i / MAP_WIDTH, game->width/4 * j / MAP_WIDTH, game->width/4 / MAP_WIDTH + 1, game->width/4 / MAP_WIDTH + 1};
                SDL_RenderFillRect(game->renderer, &wall_rect);
            }
        }
    }
    
    player_texture_rect.x -= player_texture_rect.h / 2;
    player_texture_rect.y -= player_texture_rect.h / 2;

    SDL_RenderCopyEx(game->renderer, player_texture, nullptr, &player_texture_rect, player_sprite_angle, nullptr, SDL_FLIP_NONE);

    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);

    for (Entity* entity : game->entities_manager.entities) {
        SDL_Rect rect1 = {static_cast<int>(entity->position_x * game->width/4 / MAP_WIDTH - 3), static_cast<int>(entity->position_y * game->width/4 / MAP_WIDTH - 3), 6, 6};
        SDL_RenderFillRect(game->renderer, &rect1);
    }
}
