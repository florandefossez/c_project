#include <SFML/Graphics.hpp>
#include <cmath>
#include "headers/player.hpp"
#include "headers/game.hpp"


Player::Player() {

    player_texture.loadFromFile("./ressources/player.png");
    player_sprite.setOrigin(16,16);
    player_sprite.setTexture(player_texture);
    player_sprite.setPosition(sf::Vector2f(100,100));

}


void Player::update() {

    float move_x = player_sprite.getPosition().x;
    float move_y = player_sprite.getPosition().y;

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

    if (move_x < 16)
        move_x = 16;
    if (move_x > WINDOW_WIDTH - 16)
        move_x = WINDOW_WIDTH - 16;
    if (move_y < 16)
        move_y = 16;
    if (move_y > WINDOW_HEIGHT - 16)
        move_y = WINDOW_HEIGHT - 16;

    player_sprite.setPosition(move_x, move_y);

    
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