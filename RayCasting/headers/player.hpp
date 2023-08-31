#pragma once
#include <cmath>

constexpr float PLAYER_VELOCITY = 3.f;
constexpr float PLAYER_ROTATION_VELOCITY = 90.0;


struct cell_;
typedef struct cell_ cell_t;

class Game;

class Player {

public:
    Player(Game* game);
    void draw();
    void update();
    void shoot();
    float get_angle();

    float position_x;
    float position_y;
    float position_z;

    float dir_x;
    float dir_y;

    float plane_x;
    float plane_y;

private:

    Game* game;
    void pathfind();

};