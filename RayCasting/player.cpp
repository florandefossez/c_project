#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "headers/player.hpp"
#include "headers/game.hpp"


Player::Player() {

    player_texture.loadFromFile("./ressources/player.png");
    player_sprite.setOrigin(16,16);
    player_sprite.setTexture(player_texture);
    player_sprite.setPosition(sf::Vector2f(100,100));

}

inline bool collide(std::array<std::array<cell_t, 32>, 32>* map, float x, float y) {
    return ((*map)[(unsigned int) floor((x)*32/800)][(unsigned int) floor((y)*32/800)] == wall);
}


void Player::update(std::array<std::array<cell_t, 32>, 32>* map) {

    float x = player_sprite.getPosition().x;
    float y = player_sprite.getPosition().y;

    float move_x = 0;
    float move_y = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        move_x += PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
        move_y += PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        move_x += -PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
        move_y += -PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        move_x += PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
        move_y += -PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move_x += -PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
        move_y += PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
    }

    player_sprite.setColor(sf::Color::Green);
    if (collide(map, move_x+x, y)) {
        player_sprite.setColor(sf::Color::Red);
        if (move_x>0) {
            move_x = floor((x+move_x)*32/800)*800/32 - x - 0.01;
        } else if (move_x<0) {
            move_x = ceil((x+move_x)*32/800)*800/32 - x + 0.01;
        }
    }
    if (collide(map, x, y+move_y)) {
        player_sprite.setColor(sf::Color::Red);
        if (move_y>0) {
            move_y = floor((y+move_y)*32/800)*800/32 - y - 0.01;
        } else if (move_y<0) {
            move_y = ceil((y+move_y)*32/800)*800/32 - y + 0.01;
        }
    }

    player_sprite.setPosition(move_x+x, move_y+y);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
        player_sprite.setRotation(player_sprite.getRotation() + PLAYER_ROTATION_VELOCITY);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        player_sprite.setRotation(player_sprite.getRotation() - PLAYER_ROTATION_VELOCITY);
        
    
}

void Player::draw(sf::RenderWindow* window) {
    player_sprite.setRotation(player_sprite.getRotation() + (sf::Mouse::getPosition(*window).x-WINDOW_WIDTH/2)/10);
    sf::Mouse::setPosition(sf::Vector2i(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), *window);

    window->draw(player_sprite);
}