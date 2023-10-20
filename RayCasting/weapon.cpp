#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>


#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/hud.hpp"
#include "headers/game.hpp"
#include "headers/weapon.hpp"


void Weapon::draw(SDL_Rect* rect) {
    int width, height;
    SDL_RenderGetLogicalSize(game->renderer, &width, &height);
    SDL_Rect dst = {
        width / 2 - rect->w * height / 672,
        542 * width / 957 - rect->h * height / 336,
        rect->w * height / 336,
        rect->h * height / 336
    };
    SDL_RenderCopy(game->renderer, texture, rect, &dst);
}




std::array<SDL_Rect, 6> ShotGun::shoot_rects = {
    SDL_Rect{5, 0, 79, 168},
    SDL_Rect{299, 0, 111, 168},
    SDL_Rect{209, 0, 87, 168},
    SDL_Rect{87, 0, 119, 168},
    SDL_Rect{520, 0, 79, 168},
    SDL_Rect{430, 0, 79, 168}
};

ShotGun::ShotGun(Game* game) : Weapon(game, 30.f, 10) {
    texture = IMG_LoadTexture(game->renderer, "ressources/weapons/shotgun.png");
    shoot_sound = Mix_LoadWAV("ressources/sounds/shotgun.wav");
}

bool ShotGun::update(bool tick, bool fire) {
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    if (fire && !cooldown && munitions) {
        Mix_PlayChannel(1, shoot_sound, 0);
        cooldown = 10;
        munitions--;
        return true;
    }
    return false;
}

void ShotGun::draw() {
    Weapon::draw(&ShotGun::shoot_rects[cooldown/2]);
}

float ShotGun::getdamage(float distance) {
    (void) distance;
    return damage;
}





std::array<SDL_Rect, 6> Hands::shoot_rects = {
    SDL_Rect{0, 0, 112, 75},
    SDL_Rect{117, 0, 80, 75},
    SDL_Rect{201, 0, 105, 75},
    SDL_Rect{310, 0, 145, 75},
    SDL_Rect{201, 0, 105, 75},
    SDL_Rect{117, 0, 80, 75},
};

Hands::Hands(Game* game) : Weapon(game, 20.f, 0) {
    texture = IMG_LoadTexture(game->renderer, "ressources/weapons/hands.png");
    punch = Mix_LoadWAV("ressources/sounds/punch.wav");
}

bool Hands::update(bool tick, bool fire) {
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    if (!cooldown && fire) {
        Mix_PlayChannel(1, punch, 0);
        cooldown = 10;
        return true;
    }
    return false;
}

void Hands::draw() {
    Weapon::draw(&Hands::shoot_rects[cooldown/2]);
}

float Hands::getdamage(float distance) {
    return distance < 2.f ? damage : 0.f;
}






std::array<SDL_Rect, 4> MachineGun::shoot_rects = {
    SDL_Rect{0, 0, 114, 103},
    SDL_Rect{117, 0, 114, 103},
    SDL_Rect{246, 0, 114, 103},
    SDL_Rect{363, 0, 114, 103},
};

MachineGun::MachineGun(Game* game) : Weapon(game, 4.f, 100) {
    texture = IMG_LoadTexture(game->renderer, "ressources/weapons/machineGun.png");
    spin_up = Mix_LoadWAV("ressources/sounds/minigun_spin_up.wav");
    spin_down = Mix_LoadWAV("ressources/sounds/minigun_spin_down.wav");
    shoot_sound = Mix_LoadWAV("ressources/sounds/minigun_fire.wav");
}

bool MachineGun::update(bool tick, bool fire) {
    if (tick && fire && cooldown<10) {
        if (cooldown == 0) Mix_PlayChannel(1, spin_up, 0);
        if (cooldown == 9 && munitions) Mix_PlayChannel(1, shoot_sound, -1);
        cooldown++;
        return false;
    }
    if (tick && fire && cooldown) {
        cooldown = 10 + (cooldown+1)%2;
        if (cooldown%2 && munitions) munitions--;
        if (!munitions) Mix_HaltChannel(1);
        return munitions;
    }
    if (tick && !fire && cooldown>0) {
        if (cooldown>=10) {
            Mix_HaltChannel(1);
            Mix_PlayChannel(1, spin_down, 0);
            cooldown = 10;
        }
        cooldown--;
        return false;
    }
    return false;
}

void MachineGun::draw() {
    int frame = 0;
    if (cooldown >= 10 && munitions) frame = 2+cooldown%2;
    if ((cooldown < 10 && cooldown > 0) || (!munitions && cooldown)) frame = cooldown%2;
    Weapon::draw(&MachineGun::shoot_rects[frame]);
}

float MachineGun::getdamage(float distance) {
    (void) distance;
    return damage;
}








std::array<SDL_Rect, 5> RocketLauncher::shoot_rects = {
    SDL_Rect{0, 0, 87, 119},
    SDL_Rect{405, 0, 101, 119},
    SDL_Rect{297, 0, 101, 119},
    SDL_Rect{186, 0, 101, 119},
    SDL_Rect{93, 0, 87, 119},    
};

RocketLauncher::RocketLauncher(Game* game) : Weapon(game, 40.f, 20) {
    texture = IMG_LoadTexture(game->renderer, "ressources/weapons/rocketLauncher.png");
    rocket_launch = Mix_LoadWAV("ressources/sounds/rocket_launch.wav");
}

bool RocketLauncher::update(bool tick, bool fire) {
    if (fire && cooldown==0 && munitions) {
        munitions--;
        cooldown = 6;
        Mix_PlayChannel(1, rocket_launch, 0);
        game->entities_manager.entities.push_back(
            new FireBall(game->player.position_x, game->player.position_y, game->player.dir_x*10.f, game->player.dir_y*10.f)
        );
        return false;
    }
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    return false;
}

void RocketLauncher::draw() {
    Weapon::draw(&RocketLauncher::shoot_rects[cooldown]);
}

float RocketLauncher::getdamage(float distance) {
    (void) distance;
    return 0.f;
}






std::array<SDL_Rect, 4> PlasmaGun::shoot_rects = {
    SDL_Rect{196, 0, 84, 111},
    SDL_Rect{282, 0, 85, 111},   
    SDL_Rect{0, 0, 83, 111},
    SDL_Rect{86, 0, 104, 111},
};

PlasmaGun::PlasmaGun(Game* game) : Weapon(game, 20.f, 60) {
    texture = IMG_LoadTexture(game->renderer, "ressources/weapons/plasmaGun.png");
    shot = Mix_LoadWAV("ressources/sounds/plasma.wav");
}

bool PlasmaGun::update(bool tick, bool fire) {
    if (tick && fire && cooldown>=10 && munitions) {
        if (cooldown == 13) {
            Mix_PlayChannel(1, shot, 0);
            game->entities_manager.entities.push_back(
                new PlasmaBall(game->player.position_x, game->player.position_y, game->player.dir_x*10.f, game->player.dir_y*10.f)
            );
            munitions--;
        }
        cooldown = 10 + (cooldown+1)%4;
        return false;
    }
    if (fire && cooldown==0 && munitions) {
        cooldown = 12;
        Mix_PlayChannel(1, shot, 0);
        game->entities_manager.entities.push_back(
            new PlasmaBall(game->player.position_x, game->player.position_y, game->player.dir_x*10.f, game->player.dir_y*10.f)
        );
        munitions--;
        return false;
    }
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    return false;
}

void PlasmaGun::draw() {
    int frame;
    if (cooldown == 0) frame = 2;
    if (cooldown>0 && cooldown<10) frame = 3; 
    if (cooldown>=10) frame = (cooldown-10)/3;
    Weapon::draw(&PlasmaGun::shoot_rects[frame]);
}

float PlasmaGun::getdamage(float distance) {
    (void) distance;
    return 0.f;
}





std::array<SDL_Rect, 4> ChainSaw::shoot_rects = {
    SDL_Rect{0, 0, 140, 89},
    SDL_Rect{143, 0, 140, 89},   
    SDL_Rect{286, 0, 153, 89},
    SDL_Rect{442, 0, 154, 89},
};

ChainSaw::ChainSaw(Game* game) : Weapon(game, 4.f, 0) {
    texture = IMG_LoadTexture(game->renderer, "ressources/weapons/chainSaw.png");
    sawing = Mix_LoadWAV("ressources/sounds/chainsaw.wav");
    idle = Mix_LoadWAV("ressources/sounds/chainsaw_idle.wav");
}

bool ChainSaw::update(bool tick, bool fire) {
    if (fire && tick) {
        if (cooldown<2) {
            Mix_HaltChannel(1);
            Mix_PlayChannel(1, sawing, -1);
        }
        cooldown = 2 + (cooldown+1)%2;
        return true;
    }
    if (tick) {
        if (cooldown>=2) {
            Mix_HaltChannel(1);
            Mix_PlayChannel(1, idle, -1);
        }
        cooldown = (cooldown+1)%2;
    }
    return false;
}

void ChainSaw::draw() {
    Weapon::draw(&ChainSaw::shoot_rects[cooldown]);
}

float ChainSaw::getdamage(float distance) {
    return distance < 2.f ? damage : 0.f;
}