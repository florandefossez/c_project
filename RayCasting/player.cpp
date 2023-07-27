#include <SFML/Graphics.hpp>
#include <iostream>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/game.hpp"


Player::Player(Game* game) : game(game) {

    position_x = 2.0;
    position_y = 3.0;
    position_z = 0.5;

    dir_x = 1.0;
    dir_y = 0.0;

    plane_x = 0.0;
    plane_y = 0.7;
}

float Player::get_angle() {
    float angle = atanf(dir_y/dir_x)*180.0/3.14159;
    if (dir_x < 0) angle -= 180;
    if (angle < 0) angle += 360;
    return angle;
}

void Player::update() {

    float move_x = 0;
    float move_y = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        move_x += PLAYER_VELOCITY*dir_x;
        move_y += PLAYER_VELOCITY*dir_y;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        move_x += -PLAYER_VELOCITY*dir_x;
        move_y += -PLAYER_VELOCITY*dir_y;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        move_x += PLAYER_VELOCITY*dir_y;
        move_y += -PLAYER_VELOCITY*dir_x;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move_x += -PLAYER_VELOCITY*dir_y;
        move_y += PLAYER_VELOCITY*dir_x;
    }

    if (game->map.collide(move_x+position_x, position_y)) {
        if (move_x>0) {
            move_x = floor((position_x+move_x)) - position_x - 0.01;
        } else if (move_x<0) {
            move_x = ceil((position_x+move_x)) - position_x + 0.01;
        }
    }
    if (game->map.collide(position_x, position_y+move_y)) {
        if (move_y>0) {
            move_y = floor((position_y+move_y)) - position_y - 0.01;
        } else if (move_y<0) {
            move_y = ceil((position_y+move_y)) - position_y + 0.01;
        }
    }
    position_x += move_x;
    position_y += move_y;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        float tmp_dir = COS_PLAYER_ROTATION*dir_x - SIN_PLAYER_ROTATION*dir_y;
        dir_y = SIN_PLAYER_ROTATION*dir_x + COS_PLAYER_ROTATION*dir_y;
        dir_x = tmp_dir;

        tmp_dir = COS_PLAYER_ROTATION*plane_x - SIN_PLAYER_ROTATION*plane_y;
        plane_y = SIN_PLAYER_ROTATION*plane_x + COS_PLAYER_ROTATION*plane_y;
        plane_x = tmp_dir;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        float tmp_dir = COS_PLAYER_ROTATION*dir_x + SIN_PLAYER_ROTATION*dir_y;
        dir_y = - SIN_PLAYER_ROTATION*dir_x + COS_PLAYER_ROTATION*dir_y;
        dir_x = tmp_dir;

        tmp_dir = COS_PLAYER_ROTATION*plane_x + SIN_PLAYER_ROTATION*plane_y;
        plane_y = - SIN_PLAYER_ROTATION*plane_x + COS_PLAYER_ROTATION*plane_y;
        plane_x = tmp_dir;
    }
        
}


void Player::draw() {

}

