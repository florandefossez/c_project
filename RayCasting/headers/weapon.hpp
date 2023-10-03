#pragma once
#include <vector>
#include <map>
#include <string>
#include <SDL2/SDL_mixer.h>


class Game;

class Weapon {
    public:

    float damage;
    unsigned int munitions;
    unsigned int cooldown;
    bool available;
    SDL_Texture* texture;

    Weapon(Game* game, float damage, unsigned int munitions) : damage(damage), munitions(munitions), cooldown(0), available(false), game(game) {};
    virtual ~Weapon() {SDL_DestroyTexture(texture);};

    void draw(SDL_Rect* rect);

    virtual void draw() = 0;
    virtual bool update(bool tick, bool fire) = 0;

    protected:

    Game* game;
};



class Hands : public Weapon {
    public:

    Hands(Game* game);
    ~Hands() override {};

    static std::array<SDL_Rect, 6> shoot_rects;

    void draw() override;
    bool update(bool tick, bool fire) override;

    private:

    Mix_Chunk* punch;
};


class ShotGun : public Weapon {
    public:

    ShotGun(Game* game);
    ~ShotGun() override {Mix_FreeChunk(shoot_sound);};

    static std::array<SDL_Rect, 6> shoot_rects;

    void draw() override;
    bool update(bool tick, bool fire) override;

    private:

    Mix_Chunk* shoot_sound;
};


class MachineGun : public Weapon {
    public:

    MachineGun(Game* game);
    ~MachineGun() override {
        Mix_FreeChunk(spin_up);
        Mix_FreeChunk(spin_down);
        Mix_FreeChunk(shoot_sound);
    };

    static std::array<SDL_Rect, 4> shoot_rects;

    void draw() override;
    bool update(bool tick, bool fire) override;

    private:

    Mix_Chunk* spin_up;
    Mix_Chunk* spin_down;
    Mix_Chunk* shoot_sound;
};


class RocketLauncher : public Weapon {
    public:

    RocketLauncher(Game* game);
    ~RocketLauncher() override {Mix_FreeChunk(rocket_launch);};

    static std::array<SDL_Rect, 5> shoot_rects;

    void draw() override;
    bool update(bool tick, bool fire) override;

    private:

    Mix_Chunk* rocket_launch;
};


class PlasmaGun : public Weapon {
    public:

    PlasmaGun(Game* game);
    ~PlasmaGun() override {};

    static std::array<SDL_Rect, 4> shoot_rects;

    void draw() override;
    bool update(bool tick, bool fire) override;

    private:

    Mix_Chunk* shot;
};


class ChainSaw : public Weapon {
    public:

    ChainSaw(Game* game);
    ~ChainSaw() override {};

    static std::array<SDL_Rect, 4> shoot_rects;

    void draw() override;
    bool update(bool tick, bool fire) override;

    private:

    Mix_Chunk* idle;
    Mix_Chunk* sawing;
};
