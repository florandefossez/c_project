#pragma once
#include <cmath>

constexpr float PLAYER_VELOCITY = 4.f;
constexpr float PLAYER_ROTATION_VELOCITY = 90.0;


struct cell_;
typedef struct cell_ cell_t;

typedef enum player_state_ {
    CALM,
    CLENCH,
    SCREAM,
    LAUGHT
} player_state_t;

class Game;
class Weapon;

class Player {

public:
    Player(Game* game);
    void draw();
    void update();
    void shoot();
    void load();
    void start(int level_id);
    void rotate(float relative_mov);
    float get_angle();
    void damage(float value);

    float position_x;
    float position_y;
    float position_z;

    float dir_x;
    float dir_y;

    float plane_x;
    float plane_y;
    
    float health;

    player_state_t state;

    Weapon* weapon;
    Weapon* weapons[6];

private:

    Game* game;
    unsigned int state_change_cooldown;
    void pathfind();
};