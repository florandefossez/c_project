#pragma once

#include <vector>

class Game;

class Entity {
public:
    Entity(float x, float y);

    float position_x;
    float position_y;
    
    float camera_x;
    float camera_y;
};

class Object_manager {
public:
    Object_manager(Game* game);

    void update();
    void draw();

private:

    Game* game;
    sf::Sprite sprite_barrel;
    sf::Texture barrel_texture;

    std::vector<Entity> entities;

};

