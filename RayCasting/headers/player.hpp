#pragma once
#include <cmath>

constexpr float PLAYER_VELOCITY = 0.15;
constexpr float PLAYER_ROTATION_VELOCITY = 3.0;

float const SIN_PLAYER_ROTATION = sinf(PLAYER_ROTATION_VELOCITY/180.0*3.14159);
float const COS_PLAYER_ROTATION = cosf(PLAYER_ROTATION_VELOCITY/180.0*3.14159);


enum cell_ : short;
typedef enum cell_ cell_t;

class Game;

class Player {

public:
    Player(Game* game);
    void draw();
    void update();

    float position_x;
    float position_y;
    float position_z;

    float dir_x;
    float dir_y;

private:
    void ray_casting();

    Game* game;

    sf::Texture brick_texture;
    sf::Image stone;
    sf::Image mosse;
    float rays_lenght[1080];
    float texture_offset[1080];

    float plane_x;
    float plane_y;
};