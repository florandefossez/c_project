#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"

Map::Map(Game* game) : game(game) {
    load_map();

    player_texture = SDL_LoadBMP("ressources/player.bmp");
    player_texture = SDL_ConvertSurfaceFormat(player_texture, SDL_PIXELFORMAT_RGBA32, 0);
    player_texture_rect.x = 0;
    player_texture_rect.y = 0;
    player_texture_rect.w = player_texture->w;
    player_texture_rect.h = player_texture->h;
    player_texture_width = player_texture->w;
    player_texture_height = player_texture->h;

    vision_field_points.resize(WINDOW_WIDTH + 1);
}

void Map::load_map() {
    SDL_Surface* map_surface = SDL_LoadBMP("ressources/map.bmp");
    map_texture = SDL_CreateTextureFromSurface(game->renderer, map_surface);
    SDL_FreeSurface(map_surface);

    for (unsigned int i = 0; i < 32; i++) {
        for (unsigned int j = 0; j < 32; j++) {
            Uint32 pixel;
            Uint8 r, g, b, a;
            SDL_GetRenderDrawColor(game->renderer, &r, &g, &b, &a);
            SDL_RenderReadPixels(game->renderer, &SDL_Rect{static_cast<int>(i), static_cast<int>(j), 1, 1}, 0, &pixel);
            SDL_GetRGBA(pixel, game->renderer->format, &r, &g, &b, &a);

            if (a == 255) {
                this->map[i][j] = wall;
            } else {
                this->map[i][j] = empty;
            }
        }
    }
}

bool Map::collide(float x, float y) {
    return (map[static_cast<unsigned int>(floor(x))][static_cast<unsigned int>(floor(y))] == wall);
}

bool Map::collide(unsigned int x, unsigned int y) {
    return (map[x][y] == wall);
}

void Map::update() {
    player_sprite_rect.x = static_cast<int>(game->player.position_x * MINIMAP / 32.0);
    player_sprite_rect.y = static_cast<int>(game->player.position_y * MINIMAP / 32.0);
    player_sprite_rect.w = player_texture_width;
    player_sprite_rect.h = player_texture_height;
    player_sprite_angle = static_cast<double>(game->player.get_angle());
}

void Map::draw() {
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

    for (unsigned int i = 0; i < 32; i++) {
        for (unsigned int j = 0; j < 32; j++) {
            if (collide(i, j)) {
                SDL_Rect wall_rect = {static_cast<int>(MINIMAP / 32.0 * i), static_cast<int>(MINIMAP / 32.0 * j), static_cast<int>(MINIMAP / 32.0), static_cast<int>(MINIMAP / 32.0)};
                SDL_RenderFillRect(game->renderer, &wall_rect);
            }
        }
    }

    int player_center_x = player_sprite_rect.x + player_sprite_rect.w / 2;
    int player_center_y = player_sprite_rect.y + player_sprite_rect.h / 2;

    for (int i = 0; i <= WINDOW_WIDTH; i++) {
        int x = static_cast<int>(player_center_x + (cos(player_sprite_angle - FOV / 2 + FOV * i / WINDOW_WIDTH) * MINIMAP));
        int y = static_cast<int>(player_center_y + (sin(player_sprite_angle - FOV / 2 + FOV * i / WINDOW_WIDTH) * MINIMAP));

        vision_field_points[i].x = static_cast<float>(x);
        vision_field_points[i].y = static_cast<float>(y);
    }

    SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
    SDL_RenderDrawLines(game->renderer, &vision_field_points[0], WINDOW_WIDTH + 1);
    SDL_RenderCopyEx(game->renderer, player_texture, nullptr, &player_sprite_rect, player_sprite_angle * 180.0 / M_PI, nullptr, SDL_FLIP_NONE);
}
