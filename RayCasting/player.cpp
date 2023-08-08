#include <iostream>
#include <SDL2/SDL.h>
#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
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

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_W]) {
        move_x += PLAYER_VELOCITY*dir_x;
        move_y += PLAYER_VELOCITY*dir_y;
    }

    if (keyboardState[SDL_SCANCODE_S]) {
        move_x += -PLAYER_VELOCITY*dir_x;
        move_y += -PLAYER_VELOCITY*dir_y;
    }
    
    if (keyboardState[SDL_SCANCODE_A]) {
        move_x += PLAYER_VELOCITY*dir_y;
        move_y += -PLAYER_VELOCITY*dir_x;
    }
    
    if (keyboardState[SDL_SCANCODE_D]) {
        move_x += -PLAYER_VELOCITY*dir_y;
        move_y += PLAYER_VELOCITY*dir_x;
    }

    move_x *= game->delta_time;
    move_y *= game->delta_time;

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

    float const SIN_PLAYER_ROTATION = sinf(PLAYER_ROTATION_VELOCITY/180.0*3.14159 * game->delta_time);
    float const COS_PLAYER_ROTATION = cosf(PLAYER_ROTATION_VELOCITY/180.0*3.14159 * game->delta_time);

    if (keyboardState[SDL_SCANCODE_RIGHT]) {
        float tmp_dir = COS_PLAYER_ROTATION*dir_x - SIN_PLAYER_ROTATION*dir_y;
        dir_y = SIN_PLAYER_ROTATION*dir_x + COS_PLAYER_ROTATION*dir_y;
        dir_x = tmp_dir;

        tmp_dir = COS_PLAYER_ROTATION*plane_x - SIN_PLAYER_ROTATION*plane_y;
        plane_y = SIN_PLAYER_ROTATION*plane_x + COS_PLAYER_ROTATION*plane_y;
        plane_x = tmp_dir;
    }
    
    if (keyboardState[SDL_SCANCODE_LEFT]) {
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

