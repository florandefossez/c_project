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
    Entity(float x, float y, float size) : position_x(x), position_y(y), size(size) {};
    virtual ~Entity() {};

    float position_x;
    float position_y;
    
    float camera_x;
    float camera_y;

    float size;

    SDL_Surface* surface;

    void draw(Game* game, SDL_Rect& rect);
    virtual void draw(Game* game);
    virtual void update(Game* game);
    virtual void damage(float value);
};

class Barrel : public Entity {
public:
    Barrel(float x, float y);
    void damage(float value) {Entity::damage(value);};
};

class Enemy : public Entity {
public:
    Enemy(float x, float y, float size) : Entity(x,y,size), animation_cooldown(0), velocity(2) {};
    virtual ~Enemy() {};
    void update(Game* game) override;
    virtual void death() {};

    int health;
    int damage;
    unsigned int animation_cooldown;
    float accuracy;
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


    static std::array<SDL_Rect, 4> walk_front_rects;
    static std::array<SDL_Rect, 2> shoot_rects;
    static std::array<SDL_Rect, 1> pain_rect;
    static std::array<SDL_Rect, 9> death1;
    // static std::array<sf::IntRect, 6> death2;

    void update(Game* game) override;
    void draw(Game* game) override;

    void damage(float value) override;
    void death() override;

    
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

