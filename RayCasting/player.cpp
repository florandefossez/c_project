#include <iostream>
#include <SDL2/SDL.h>
#include <queue>
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

    pathfind();
        
}


void Player::draw() {

}



void Player::pathfind() {
    for (int i=0; i<32; i++) {
        for (int j=0; j<32; j++) {
            game->map.map[i][j].dir = 0;
        }
    }

    int p_x = static_cast<int>(position_x);
    int p_y = static_cast<int>(position_y);

    std::queue<std::pair<int,int>> queue;
    queue.push({p_x, p_y});
    int i,j;
    while (!queue.empty()) {
        std::tie(i,j) = queue.front();
        queue.pop();
        if (std::abs(p_x-i)>20 || std::abs(p_y-j)>20)
            continue;
        
        int a = i-1, b = j;
        if (a>=0 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 3;
            queue.push({a,b});
        }
        a = i+1, b = j;
        if (a<32 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 1;
            queue.push({a,b});
        }
        a = i, b = j-1;
        if (b>=0 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 4;
            queue.push({a,b});
        }
        a = i, b = j+1;
        if (b<32 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 2;
            queue.push({a,b});
        }
    }
}