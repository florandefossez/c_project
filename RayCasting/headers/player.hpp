#pragma once

constexpr float PLAYER_VELOCITY = 0.15;
constexpr float PLAYER_ROTATION_VELOCITY = 3.0;


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
};