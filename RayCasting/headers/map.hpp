#pragma once

#include <array>

typedef enum cell_ : short {
    empty,
    wall,
} cell_t;

class Game;

class Map {
public:
    Map(Game* game);
    bool collide(unsigned int x, unsigned int y);
    bool collide(float x, float y);
    void update();
    void draw();

    sf::Sprite player_sprite;
    sf::Texture player_texture;
    sf::VertexArray vision_field;

private:
    std::array<std::array<cell_t,32>,32> map;
    void load_map();
    Game* game;
};