#pragma once
#include <vector>
#include <map>
#include <string>

class Game;

class Weapon {
    public:

    float damage;
    unsigned int munitions;
    unsigned int cooldown;
    SDL_Texture* texture;

    Weapon(float damage, unsigned int munitions) : damage(damage), munitions(munitions), cooldown(0) {};
    virtual ~Weapon() {SDL_DestroyTexture(texture);};

    void draw(SDL_Renderer* renderer, SDL_Rect* rect);

    virtual void draw(SDL_Renderer* renderer) = 0;
    virtual void update(bool tick) = 0;
    virtual void shoot() = 0;
    virtual bool can_shoot() = 0;

};


class ShotGun : public Weapon {
    public:

    ShotGun(SDL_Renderer* renderer);
    ~ShotGun() override {};

    static std::array<SDL_Rect, 6> shoot_rects;

    void draw(SDL_Renderer* renderer) override;
    void update(bool tick) override;
    void shoot() override;
    bool can_shoot() override;
};



class Hands : public Weapon {
    public:

    Hands(SDL_Renderer* renderer);
    ~Hands() override {};

    static std::array<SDL_Rect, 6> shoot_rects;

    void draw(SDL_Renderer* renderer) override;
    void update(bool tick) override;
    void shoot() override;
    bool can_shoot() override;
};