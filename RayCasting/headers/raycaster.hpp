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
    void start();
    void update_width();

    void trigger();

    int targeted_wall_x;
    int targeted_wall_y;

private:

    void draw_wall();
    void draw_floor();

    Game* game;

    // textures
    Uint32* textures[20]; 

    int opening_door_x;
    int opening_door_y;
    float opening_state;

};