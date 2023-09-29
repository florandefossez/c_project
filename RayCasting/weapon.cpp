#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>


#include "headers/weapon.hpp"
#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/hud.hpp"
#include "headers/game.hpp"


std::array<SDL_Rect, 6> ShotGun::shoot_rects = {
    SDL_Rect{5, 0, 79, 168},
    SDL_Rect{299, 0, 111, 168},
    SDL_Rect{209, 0, 87, 168},
    SDL_Rect{87, 0, 119, 168},
    SDL_Rect{520, 0, 79, 168},
    SDL_Rect{430, 0, 79, 168}
};


ShotGun::ShotGun(SDL_Renderer* renderer) : Weapon(30.f, 100) {
    texture = IMG_LoadTexture(renderer, "ressources/weapons/shotgun.png");
}

void ShotGun::update(bool tick) {
    static int s = 0;
    if (tick) {
        if (cooldown && s<=0) {
            cooldown--;
            s = 2;
        } else {
            s--;
        };
    }
}

void ShotGun::draw(SDL_Renderer* renderer) {
    int width, height;
    SDL_RenderGetLogicalSize(renderer, &width, &height);
    SDL_Rect dst = {
        width / 2 - ShotGun::shoot_rects[cooldown].w * height / 672,
        height / 2 - (32 * width / 319),
        ShotGun::shoot_rects[cooldown].w * height / 336,
        height / 2
    };
    SDL_RenderCopy(renderer, texture, &ShotGun::shoot_rects[cooldown], &dst);
}

void ShotGun::shoot() {
    cooldown = 5;
}