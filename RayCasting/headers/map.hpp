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
    ~Map();
    bool collide(unsigned int x, unsigned int y);
    bool collide(float x, float y);
    void update();
    void draw();

    SDL_Rect player_sprite_rect;
    SDL_Texture* player_texture;
    SDL_Point vision_field_points[WINDOW_WIDTH + 1];

private:
    std::array<std::array<cell_t, 32>, 32> map;
    void load_map();
    Game* game;
    int player_texture_width;
    int player_texture_height;
    double player_sprite_angle;
    SDL_Texture* map_texture;
};
