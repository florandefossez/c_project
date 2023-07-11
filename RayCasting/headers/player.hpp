#pragma once

constexpr int PLAYER_VELOCITY = 5;
constexpr int PLAYER_ROTATION_VELOCITY = 5;


enum cell_ : short;
typedef enum cell_ cell_t;

class Player {

public:
    Player();
    void draw(sf::RenderWindow* window);
    void update(std::array<std::array<cell_t, 32>, 32>* map);

private:
    sf::Sprite player_sprite;
    sf::Texture player_texture;

};