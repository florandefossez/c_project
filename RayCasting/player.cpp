#include <iostream>
#include <SDL2/SDL.h>
#include <queue>
#include <vector>
#include <algorithm>
#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/weapon.hpp"
#include "headers/hud.hpp"
#include "headers/game.hpp"


Player::Player(Game* game) : game(game) {}

void Player::load() {
    weapons[0] = new Hands(game);
    weapons[1] = new ShotGun(game);
    weapons[2] = new MachineGun(game);
    weapons[3] = new RocketLauncher(game);
    weapons[4] = new PlasmaGun(game);
    weapons[5] = new ChainSaw(game);
}

void Player::start(int level_id) {
    blood_level = 0;
    frag = 0;
    game_over = 0;
    health = 100.f;
    state_change_cooldown = 0;

    for (auto weap : weapons) {
        weap->munitions = 0;
        weap->cooldown = 0;
        weap->available = false;
    }
    weapons[0]->available = true;

    switch (level_id) {
    case 0:
        position_x = 2;
        position_y = 2;
        position_z = 0.5;
        dir_x = cos(95.f);
        dir_y = sin(95.f);
        weapon = 0;
        break;
    
    case 1:
        position_x = 5.5;
        position_y = 38.5;
        position_z = 0.5;
        dir_x = cos(-80.f);
        dir_y = sin(-80.f);
        weapon = 0;
        break;

    }

    plane_x = -0.7*dir_y;
    plane_y = 0.7*dir_x;
}


float Player::get_angle() {
    float angle = atanf(dir_y/dir_x)*180.0/3.14159;
    if (dir_x < 0) angle -= 180;
    if (angle < 0) angle += 360;
    return angle;
}

void Player::damage(float value) {
    health -= value;
    blood_level += blood_level>100 ? 0 : 10;
    if (!state_change_cooldown) {
        state_change_cooldown = 20;
        state = player_state_t::CLENCH;
    }
}

void Player::update() {

    if (game_over) {
        if (game->animation) game_over--;
        if (!game_over) game->menu();
        return;
    }
    if (health <= 0) game_over = 80;
    if (blood_level && game->animation) blood_level--;

    float move_x = 0;
    float move_y = 0;

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_GetScancodeFromKey(game->move_forward)]) {
        move_x += PLAYER_VELOCITY*dir_x;
        move_y += PLAYER_VELOCITY*dir_y;
    }

    if (keyboardState[SDL_GetScancodeFromKey(game->move_backward)]) {
        move_x += -PLAYER_VELOCITY*dir_x;
        move_y += -PLAYER_VELOCITY*dir_y;
    }
    
    if (keyboardState[SDL_GetScancodeFromKey(game->move_left)]) {
        move_x += PLAYER_VELOCITY*dir_y;
        move_y += -PLAYER_VELOCITY*dir_x;
    }
    
    if (keyboardState[SDL_GetScancodeFromKey(game->move_right)]) {
        move_x += -PLAYER_VELOCITY*dir_y;
        move_y += PLAYER_VELOCITY*dir_x;
    }

    move_x *= game->delta_time;
    move_y *= game->delta_time;

    position_x += move_x;
    if (game->map.collide(position_x + 0.1f, position_y))
        position_x = floor(position_x + 0.1f) - 0.1f;

    if (game->map.collide(position_x - 0.1f, position_y))
        position_x = ceil(position_x - 0.1f) + 0.1f;

    position_y += move_y;
    if (game->map.collide(position_x, position_y + 0.1f))
        position_y = floor(position_y + 0.1f) - 0.1f;
    
    if (game->map.collide(position_x, position_y - 0.1f))
        position_y = ceil(position_y - 0.1f) + 0.1f;

#ifndef __EMSCRIPTEN__
    if (keyboardState[SDL_SCANCODE_RIGHT]) rotate(game->delta_time);
    if (keyboardState[SDL_SCANCODE_LEFT]) rotate(-game->delta_time);
#endif

    pathfind();
    if (weapons[weapon]->update(game->animation, SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(game->shoot)])) {
        if (game->entities_manager.targeted_entity) {
            game->entities_manager.targeted_entity->damage(weapons[weapon]->getdamage(game->entities_manager.targeted_entity_distance));
            if (state_change_cooldown == 0) {
                state_change_cooldown = 20;
                state = player_state_t::SCREAM;
            }
        }
    }

    if (state_change_cooldown == 0) {
        state = player_state_t::CALM;
    }
    if (state_change_cooldown && game->animation) state_change_cooldown--;
}


void Player::rotate(float relative_mov) {
    float const SIN_PLAYER_ROTATION = sinf(PLAYER_ROTATION_VELOCITY/180.0*3.14159 * relative_mov);
    float const COS_PLAYER_ROTATION = cosf(PLAYER_ROTATION_VELOCITY/180.0*3.14159 * relative_mov);

    float tmp_dir = COS_PLAYER_ROTATION*dir_x - SIN_PLAYER_ROTATION*dir_y;
    dir_y = SIN_PLAYER_ROTATION*dir_x + COS_PLAYER_ROTATION*dir_y;
    dir_x = tmp_dir;

    tmp_dir = COS_PLAYER_ROTATION*plane_x - SIN_PLAYER_ROTATION*plane_y;
    plane_y = SIN_PLAYER_ROTATION*plane_x + COS_PLAYER_ROTATION*plane_y;
    plane_x = tmp_dir;
}


void Player::switch_weapon() {
    Mix_HaltChannel(1);
    do {
        weapon = (weapon + 1) % 6;
    } while (!weapons[weapon]->available);
}


void Player::draw() {
    if (blood_level) {
        SDL_SetRenderDrawColor(game->renderer, 0xbb, 0x0a, 0x1e, blood_level);
        SDL_RenderFillRect(game->renderer, nullptr);
    }
    weapons[weapon]->draw();
}



void Player::pathfind() {
    for (int i=0; i<64; i++) {
        for (int j=0; j<64; j++) {
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
        if (a<64 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 1;
            queue.push({a,b});
        }
        a = i, b = j-1;
        if (b>=0 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 4;
            queue.push({a,b});
        }
        a = i, b = j+1;
        if (b<64 && !game->map.map[a][b].is_wall && game->map.map[a][b].dir == 0) {
            game->map.map[a][b].dir = 2;
            queue.push({a,b});
        }
    }
}