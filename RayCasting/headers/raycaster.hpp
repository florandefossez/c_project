#pragma once
#include <array>

class Game;

class Raycaster {
public:
    Raycaster(Game* game);
    void update();
    void draw();

    float rays_lenght[1080];

    void load();

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
    SDL_Texture* scene;

    float perp_rays_lenght[1080];
    float texture_offset[1080];

    std::array<uint32_t, 1080 * 720> scene_pixels;
};