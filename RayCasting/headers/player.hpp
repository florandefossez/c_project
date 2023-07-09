#pragma once

constexpr int PLAYER_VELOCITY = 5;
constexpr int PLAYER_ROTATION_VELOCITY = 5;


class Player {

public:
    Player();
    void draw(sf::RenderWindow* window);
    void update();

private:
    sf::Sprite player_sprite;
    sf::Texture player_texture;

};