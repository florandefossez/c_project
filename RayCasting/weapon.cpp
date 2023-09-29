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
}

void ShotGun::update(bool tick) {
    if (tick && cooldown) {
        cooldown--;
    }
}

void ShotGun::draw(SDL_Renderer* renderer) {
    Weapon::draw(renderer, &ShotGun::shoot_rects[cooldown/2]);
}

void ShotGun::shoot() {
    cooldown = 11;
    munitions--;
}

bool ShotGun::can_shoot() {
    return (cooldown == 0 && munitions > 0);
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

void Hands::update(bool tick) {
    if (tick && cooldown) {
        cooldown--;
    }
}

void Hands::draw(SDL_Renderer* renderer) {
    Weapon::draw(renderer, &Hands::shoot_rects[cooldown/2]);
}

void Hands::shoot() {
    cooldown = 11;
}

bool Hands::can_shoot() {
    return (cooldown == 0);
}