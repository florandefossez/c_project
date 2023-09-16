#pragma once
#include <array>

class Game;

class Raycaster {
public:
    Raycaster(Game* game);
    void update();
    void draw();

    float* rays_lenght;

    void load();
    void update_width();

private:
    void raycast_wall();

    void draw_wall();
    void draw_floor();

    Game* game;

    // floor
    SDL_Surface* stone_surface;
    SDL_Surface* mosse_surface;

    // wall
    SDL_Surface* brick_surface;

    float* perp_rays_lenght;
    float* texture_offset;

};