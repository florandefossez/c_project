#include <iostream>
#include <SDL2/SDL.h>
#include <queue>
#include <vector>
#include <algorithm>
#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"
#include "headers/weapon.hpp"
#include "headers/hud.hpp"


Player::Player(Game* game) : game(game) {

    position_x = 5.5;
    position_y = 38.5;
    position_z = 0.5;

    dir_x = cos(-80.f);
    dir_y = sin(-80.f);

    plane_x = -0.7*dir_y;
    plane_y = 0.7*dir_x;

    health = 100.f;
    state_change_cooldown = 0;
}

void Player::load() {
    weapon = new ShotGun(game->renderer);
    hud = new Hud(game->renderer);
}

float Player::get_angle() {
    float angle = atanf(dir_y/dir_x)*180.0/3.14159;
    if (dir_x < 0) angle -= 180;
    if (angle < 0) angle += 360;
    return angle;
}

void Player::damage(float value) {
    health -= value;
    if (!state_change_cooldown) {
        state_change_cooldown = 20;
        state = player_state_t::CLENCH;
    }
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
    weapon->update(game->animation);
    hud->update(game->animation, this);
    if (state_change_cooldown == 0) {
        state = player_state_t::CALM;
    }
    if (state_change_cooldown && game->animation) state_change_cooldown--;
}

void Player::shoot() {
    if (!weapon->can_shoot()) {return;}
    weapon->shoot();
    if (game->entities_manager.targeted_entity) {
        auto iter = std::find(game->entities_manager.entities.begin(), game->entities_manager.entities.end(), game->entities_manager.targeted_entity);
        if (iter != game->entities_manager.entities.end()) {
            (*iter)->damage(weapon->damage);
            if (state_change_cooldown == 0) {
                state_change_cooldown = 20;
                state = player_state_t::SCREAM;
            }
        }
    }
}

void Player::draw() {
    hud->draw(game->renderer);
    weapon->draw(game->renderer);
}



void Player::pathfind() {
    for (int i=0; i<MAP_WIDTH; i++) {
        for (int j=0; j<MAP_WIDTH; j++) {
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
        if (a<MAP_WIDTH && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 1;
            queue.push({a,b});
        }
        a = i, b = j-1;
        if (b>=0 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 4;
            queue.push({a,b});
        }
        a = i, b = j+1;
        if (b<MAP_WIDTH && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 2;
            queue.push({a,b});
        }
    }
}