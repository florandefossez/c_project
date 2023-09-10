#pragma once

#include <vector>
#include <map>
#include <string>

typedef enum npc_status_ {
    WALK,
    PAIN,
    SHOOT,
    WAIT,
    DIYING
} npc_status_t;

class Game;

class Entity {
public:
    Entity(float x, float y, float size, float health) : position_x(x), position_y(y), size(size), health(health) {};
    virtual ~Entity() {};

    float position_x;
    float position_y;
    
    float camera_x;
    float camera_y;

    float size;
    float health;

    void draw(Game* game, SDL_Rect& rect);
    virtual void draw(Game* game);
    virtual void update(Game* game) = 0;
    virtual void damage(float value) = 0;

protected:

    SDL_Surface* surface;

};

class Barrel : public Entity {
public:
    Barrel(float x, float y);
    void damage(float value) override {(void)value;};
    void update(Game* game) override {(void)game;};
};

class Enemy : public Entity {
public:
    Enemy(float x, float y, float size, float health) : Entity(x,y,size,health), animation_cooldown(0), velocity(2) {};
    virtual ~Enemy() {};
    void update(Game* game) override;
    virtual void death() = 0;

    float damage;
    unsigned int animation_cooldown;
    bool direct_ray;
    float velocity;

    float dir_x;
    float dir_y;

    npc_status_t status;

};


class Soldier1 : public Enemy {
public:
    Soldier1(float x, float y);
    ~Soldier1() {};

    void update(Game* game) override;
    void draw(Game* game) override;

    void damage(float value) override;
    void death() override;

private:

    static std::array<SDL_Rect, 4> walk_front_rects;
    static std::array<SDL_Rect, 2> shoot_rects;
    static std::array<SDL_Rect, 1> pain_rect;
    static std::array<SDL_Rect, 9> death1;
    // static std::array<sf::IntRect, 6> death2;
    
};











class Object_manager {
public:
    Object_manager(Game* game);

    void update();
    void draw();

    static SDL_Surface* getSurface(std::string name);

    std::vector<Entity*> entities;
    Entity* targeted_entity;

private:

    Game* game;
    static std::map<std::string, SDL_Surface*> surfaces;
};

