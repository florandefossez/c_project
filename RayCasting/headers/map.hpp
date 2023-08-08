#pragma once

#include <array>
#include <SDL2/SDL.h>

typedef enum cell_ : short {
    empty,
    wall,
} cell_t;

class Game;

class Map {
public:
    Map(Game* game);
    bool collide(int x, int y);
    bool collide(float x, float y);
    void update();
    void draw();

    SDL_Rect player_texture_rect;
    SDL_Texture* player_texture;
    SDL_Surface* player_surface;
    SDL_Point vision_field_points[1080];
    void load();

private:
    std::array<std::array<cell_t, 32>, 32> map;
    Game* game;
    double player_sprite_angle;
};
