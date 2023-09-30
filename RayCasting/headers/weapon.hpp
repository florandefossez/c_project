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
    bool available;
    SDL_Texture* texture;

    Weapon(float damage, unsigned int munitions) : damage(damage), munitions(munitions), cooldown(0), available(false) {};
    virtual ~Weapon() {SDL_DestroyTexture(texture);};

    void draw(SDL_Renderer* renderer, SDL_Rect* rect);

    virtual void draw(SDL_Renderer* renderer) = 0;
    virtual bool update(bool tick, bool fire) = 0;
};



class Hands : public Weapon {
    public:

    Hands(SDL_Renderer* renderer);
    ~Hands() override {};

    static std::array<SDL_Rect, 6> shoot_rects;

    void draw(SDL_Renderer* renderer) override;
    bool update(bool tick, bool fire) override;
};


class ShotGun : public Weapon {
    public:

    ShotGun(SDL_Renderer* renderer);
    ~ShotGun() override {};

    static std::array<SDL_Rect, 6> shoot_rects;

    void draw(SDL_Renderer* renderer) override;
    bool update(bool tick, bool fire) override;
};


class MachineGun : public Weapon {
    public:

    MachineGun(SDL_Renderer* renderer);
    ~MachineGun() override {};

    static std::array<SDL_Rect, 4> shoot_rects;

    void draw(SDL_Renderer* renderer) override;
    bool update(bool tick, bool fire) override;
};


