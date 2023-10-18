#pragma once

#include <array>
#include <SDL2/SDL.h>

typedef struct cell_ {
    bool is_wall;
    bool is_door;
    int dir;
    Uint8 texture_ids[5];
} cell_t;

class Game;

class Map {
public:
    Map(Game* game);
    bool collide(int x, int y);
    bool collide(float x, float y);
    void update();
    void draw();
    void draw_sky();

    SDL_Rect player_texture_rect;
    SDL_Texture* player_texture;
    SDL_Surface* player_surface;
    void load();
    void start(int level_id);

    std::array<std::array<cell_t, 64>, 64> map;
    
private:
    Game* game;
    double player_sprite_angle;
    SDL_Texture* sky_texture;
};
