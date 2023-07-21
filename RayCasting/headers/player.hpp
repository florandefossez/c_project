#pragma once
#include <cmath>

constexpr float PLAYER_VELOCITY = 0.15;
constexpr float PLAYER_ROTATION_VELOCITY = 3.0;

float const SIN_PLAYER_ROTATION = sinf(PLAYER_ROTATION_VELOCITY/180.0*3.14159);
float const COS_PLAYER_ROTATION = cosf(PLAYER_ROTATION_VELOCITY/180.0*3.14159);


enum cell_ : short;
typedef enum cell_ cell_t;

class Player {

public:
    Player();
    void draw(sf::RenderWindow* window);
    void draw3D(sf::RenderWindow* window);
    void update(std::array<std::array<cell_t, 32>, 32>* map);

private:
    void ray_casting(std::array<std::array<cell_t, 32>, 32>* map);
    sf::Sprite player_sprite;
    sf::Texture player_texture;
    sf::VertexArray vision_field;

    float rays_lenght[1080];
    char collision_side[1080];

    float position_x;
    float position_y;
    float position_z;

    float dir_x;
    float dir_y;
    float dir_z;

    float plane_x;
    float plane_y;
    float plane_z;
};