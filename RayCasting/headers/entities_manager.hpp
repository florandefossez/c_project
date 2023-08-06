#pragma once

#include <vector>
#include <map>
#include <string>

typedef enum npc_status_ {
    WALK,
    PAIN,
    SHOOT,
    WAIT
} npc_status_t;

class Game;

class Entity {
public:
    Entity(float x, float y) : position_x(x), position_y(y) {};

    float position_x;
    float position_y;
    
    float camera_x;
    float camera_y;

    sf::Texture* texture;

    void draw(Game* game, sf::IntRect& rect, float size);
    virtual void draw(Game* game);
    virtual void update(Game* game);
};

class Barrel : public Entity {
public:
    Barrel(float x, float y);
};

class Enemy : public Entity {
public:
    Enemy(float x, float y) : Entity(x,y) {};
    void update(Game* game) override;

    int health;
    int damage;
    float accuracy;

    float dir_x;
    float dir_y;

    npc_status_t status;

};

class Soldier1 : public Enemy {
public:
    Soldier1(float x, float y);


    static std::array<sf::IntRect, 4> walk_front_rects;
    static std::array<sf::IntRect, 2> shoot_rects;
    // static sf::IntRect pain;
    // static std::array<sf::IntRect, 9> death1;
    // static std::array<sf::IntRect, 6> death2;

    void update(Game* game) override;
    void draw(Game* game) override;
    
    int state;
};











class Object_manager {
public:
    Object_manager(Game* game);

    void update();
    void draw();

    static sf::Texture* getTexture(std::string name);

private:
    Game* game;
    std::vector<Entity*> entities;

    static std::map<std::string, sf::Texture*> textures;

};

