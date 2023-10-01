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


void Weapon::draw(SDL_Renderer* renderer, SDL_Rect* rect) {
    int width, height;
    SDL_RenderGetLogicalSize(renderer, &width, &height);
    SDL_Rect dst = {
        width / 2 - rect->w * height / 672,
        542 * width / 957 - rect->h * height / 336,
        rect->w * height / 336,
        rect->h * height / 336
    };
    SDL_RenderCopy(renderer, texture, rect, &dst);
}




std::array<SDL_Rect, 6> ShotGun::shoot_rects = {
    SDL_Rect{5, 0, 79, 168},
    SDL_Rect{299, 0, 111, 168},
    SDL_Rect{209, 0, 87, 168},
    SDL_Rect{87, 0, 119, 168},
    SDL_Rect{520, 0, 79, 168},
    SDL_Rect{430, 0, 79, 168}
};

ShotGun::ShotGun(SDL_Renderer* renderer) : Weapon(30.f, 10) {
    texture = IMG_LoadTexture(renderer, "ressources/weapons/shotgun.png");
    shoot_sound = Mix_LoadWAV("ressources/sounds/shotgun.wav");
}

bool ShotGun::update(bool tick, bool fire) {
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    if (fire && !cooldown && munitions) {
        Mix_PlayChannel(-1, shoot_sound, 0);
        cooldown = 11;
        munitions--;
        return true;
    }
    return false;
}

void ShotGun::draw(SDL_Renderer* renderer) {
    Weapon::draw(renderer, &ShotGun::shoot_rects[cooldown/2]);
}






std::array<SDL_Rect, 6> Hands::shoot_rects = {
    SDL_Rect{0, 0, 112, 75},
    SDL_Rect{117, 0, 80, 75},
    SDL_Rect{201, 0, 105, 75},
    SDL_Rect{310, 0, 145, 75},
    SDL_Rect{201, 0, 105, 75},
    SDL_Rect{117, 0, 80, 75},
};

Hands::Hands(SDL_Renderer* renderer) : Weapon(20.f, 0) {
    texture = IMG_LoadTexture(renderer, "ressources/weapons/hands.png");
}

bool Hands::update(bool tick, bool fire) {
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    if (!cooldown && fire) {
        cooldown = 11;
        return true;
    }
    return false;
}

void Hands::draw(SDL_Renderer* renderer) {
    Weapon::draw(renderer, &Hands::shoot_rects[cooldown/2]);
}






std::array<SDL_Rect, 4> MachineGun::shoot_rects = {
    SDL_Rect{0, 0, 114, 103},
    SDL_Rect{117, 0, 114, 103},
    SDL_Rect{246, 0, 114, 103},
    SDL_Rect{363, 0, 114, 103},
};

MachineGun::MachineGun(SDL_Renderer* renderer) : Weapon(20.f, 0) {
    texture = IMG_LoadTexture(renderer, "ressources/weapons/machineGun.png");
    spin_up = Mix_LoadWAV("ressources/sounds/minigun_spin_up.wav");
    spin_down = Mix_LoadWAV("ressources/sounds/minigun_spin_down.wav");
    shoot_sound = Mix_LoadWAV("ressources/sounds/minigun_fire.wav");
}

bool MachineGun::update(bool tick, bool fire) {
    static int channel;
    if (tick && fire && cooldown<10) {
        if (cooldown == 0) Mix_PlayChannel(-1, spin_up, 0);
        if (cooldown == 9) channel = Mix_PlayChannel(-1, shoot_sound, -1);
        cooldown++;
        return false;
    }
    if (tick && fire && cooldown) {
        cooldown = 10 + (cooldown+1)%2;
        return true;
    }
    if (tick && !fire && cooldown>0) {
        if (cooldown>=10) {
            Mix_HaltChannel(channel);
            Mix_PlayChannel(-1, spin_down, 0);
            cooldown = 10;
        }
        cooldown--;
        return false;
    }
    return false;
}

void MachineGun::draw(SDL_Renderer* renderer) {
    int frame = 0;
    if (cooldown >= 10) frame = 2+cooldown%2;
    if (cooldown < 10 && cooldown > 0) frame = cooldown%2;
    Weapon::draw(renderer, &MachineGun::shoot_rects[frame]);
}









std::array<SDL_Rect, 5> RocketLauncher::shoot_rects = {
    SDL_Rect{0, 0, 87, 119},
    SDL_Rect{405, 0, 101, 119},
    SDL_Rect{297, 0, 101, 119},
    SDL_Rect{186, 0, 101, 119},
    SDL_Rect{93, 0, 87, 119},    
};

RocketLauncher::RocketLauncher(SDL_Renderer* renderer) : Weapon(20.f, 0) {
    texture = IMG_LoadTexture(renderer, "ressources/weapons/rocketLauncher.png");
}

bool RocketLauncher::update(bool tick, bool fire) {
    if (fire && cooldown==0) {
        cooldown = 4;
        return true;
    }
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    return false;
}

void RocketLauncher::draw(SDL_Renderer* renderer) {
    Weapon::draw(renderer, &RocketLauncher::shoot_rects[cooldown]);
}






std::array<SDL_Rect, 4> PlasmaGun::shoot_rects = {
    SDL_Rect{196, 0, 84, 111},
    SDL_Rect{282, 0, 85, 111},   
    SDL_Rect{0, 0, 83, 111},
    SDL_Rect{86, 0, 104, 111},
};

PlasmaGun::PlasmaGun(SDL_Renderer* renderer) : Weapon(20.f, 0) {
    texture = IMG_LoadTexture(renderer, "ressources/weapons/plasmaGun.png");
}

bool PlasmaGun::update(bool tick, bool fire) {
    if (tick && fire && cooldown>=10) {
        cooldown = 10 + (cooldown+1)%6;
        return true;
    }
    if (fire && cooldown==0) {
        cooldown = 10;
        return false;
    }
    if (tick && cooldown) {
        cooldown--;
        return false;
    }
    return false;
}

void PlasmaGun::draw(SDL_Renderer* renderer) {
    int frame;
    if (cooldown == 0) frame = 2;
    if (cooldown>0 && cooldown<10) frame = 3; 
    if (cooldown>=10) frame = (cooldown-10)/3;
    Weapon::draw(renderer, &PlasmaGun::shoot_rects[frame]);
}







std::array<SDL_Rect, 4> ChainSaw::shoot_rects = {
    SDL_Rect{0, 0, 140, 89},
    SDL_Rect{143, 0, 140, 89},   
    SDL_Rect{286, 0, 153, 89},
    SDL_Rect{442, 0, 154, 89},
};

ChainSaw::ChainSaw(SDL_Renderer* renderer) : Weapon(20.f, 0) {
    texture = IMG_LoadTexture(renderer, "ressources/weapons/chainSaw.png");
}

bool ChainSaw::update(bool tick, bool fire) {
    if (fire && tick) {
        cooldown = 2 + (cooldown+1)%2;
        return true;
    }
    if (tick) cooldown = (cooldown+1)%2;
    return false;
}

void ChainSaw::draw(SDL_Renderer* renderer) {
    Weapon::draw(renderer, &ChainSaw::shoot_rects[cooldown]);
}