#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>


#include "headers/raycaster.hpp"
#include "headers/weapon.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/hud.hpp"
#include "headers/game.hpp"


// initializatons

std::map<std::string, SDL_Surface*> Object_manager::surfaces;

SDL_Surface* Object_manager::getSurface(std::string name) {
    if(surfaces.find(name) != surfaces.end()){
        return surfaces[name];
    } else {
        SDL_Surface* new_surface = IMG_Load(name.c_str());
        SDL_Surface* new_formated_surface = new SDL_Surface();
        new_formated_surface = SDL_ConvertSurfaceFormat(new_surface, SDL_PIXELFORMAT_RGBA8888, 0);
        SDL_FreeSurface(new_surface);
        
        surfaces[name] = new_formated_surface;
        return surfaces[name];
    }
}

std::array<SDL_Rect, 4> Soldier1::walk_front_rects = {
    SDL_Rect{5, 12, 43, 55},
    SDL_Rect{54, 12, 43, 55},
    SDL_Rect{103, 12, 43, 55},
    SDL_Rect{151, 12, 43, 55}
};

std::array<SDL_Rect, 2> Soldier1::shoot_rects = {
    SDL_Rect{204, 14, 32, 55},
    SDL_Rect{256, 14, 32, 55}
};

std::array<SDL_Rect, 1> Soldier1::pain_rect = {
    SDL_Rect{301, 14, 32, 55}
};

std::array<SDL_Rect, 9> Soldier1::death1 = {
    SDL_Rect{9, 209, 35, 61},
    SDL_Rect{48, 209, 42, 61},
    SDL_Rect{94, 209, 48, 61},

    SDL_Rect{145, 209, 52, 61},
    SDL_Rect{201, 209, 54, 61},
    SDL_Rect{259, 209, 57, 61},

    SDL_Rect{319, 209, 56, 61},
    SDL_Rect{379, 209, 57, 61},
    SDL_Rect{439, 209, 57, 61}

};

std::array<SDL_Rect, 5> FireBall::balls_rects = {
    SDL_Rect{8, 42, 16, 16},
    SDL_Rect{27, 43, 15, 15},
    SDL_Rect{144, 6, 56, 56},
    SDL_Rect{93, 12, 50, 50},
    SDL_Rect{44, 10, 48, 48}
};

Mix_Chunk* FireBall::explode = nullptr;

std::array<SDL_Rect, 4> PlasmaBall::balls_rects = {
    SDL_Rect{0, 18, 23, 23},
    SDL_Rect{25, 0, 41, 41},
    SDL_Rect{66, 0, 41, 41},
    SDL_Rect{107, 0, 41, 41}
};

std::array<SDL_Rect, 4> Ammos::ammos_rects = {
    SDL_Rect{0, 0, 15, 7},
    SDL_Rect{16, 0, 28, 16},
    SDL_Rect{45, 0, 54, 21},
    SDL_Rect{100, 0, 32, 21} 
};


Object_manager::Object_manager(Game* game) : targeted_entity(nullptr), game(game) {
    entities.clear();
}

void Object_manager::start(int level_id) {
    for (Entity* entity : entities) {
        delete entity;
    }

    entities.clear();
    targeted_entity = nullptr;
    targeted_entity_distance = 100.f;
    
    switch (level_id) {
    case 1:
        entities.push_back(new Soldier1(4.5,44.0));
        entities.push_back(new Barrel(4.5,44.0));
        entities.push_back(new Ammos(9.5,39.5,100,1));
        entities.push_back(new Ammos(12,40,100,2));
        entities.push_back(new Ammos(10,42,100,3));
        entities.push_back(new Ammos(12,42,100,4));
        break;
    
    default:
        break;
    }
}


void Object_manager::update() {
    // transform sprite with the inverse camera matrix to get their coordinate in the camera space

    // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
    // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
    // [ planeY   dirY ]                                          [ -planeY  planeX ]

    float inv_det = 1.0 / (game->player.plane_x * game->player.dir_y - game->player.dir_x * game->player.plane_y);

    for (auto entity = entities.begin(); entity != entities.end();) {
        float sprite_x = (*entity)->position_x - game->player.position_x;
        float sprite_y = (*entity)->position_y - game->player.position_y;

        // coordinate in the camera space: from world coords to camera coords
        (*entity)->camera_x = inv_det * (game->player.dir_y * sprite_x - game->player.dir_x * sprite_y);
        (*entity)->camera_y = inv_det * (-game->player.plane_y * sprite_x + game->player.plane_x * sprite_y); // this is actually the depth inside the screen
        if ((*entity)->update(game)) {
            entity = entities.erase(entity);
        } else {
            ++entity;
        }
    }

    // sort the entities to display the furthest before the closest
    // std::sort(entities.begin(), entities.end(),);
    entities.sort([] (Entity* a, Entity* b) {return a->camera_y > b->camera_y;});

    targeted_entity = nullptr;
    targeted_entity_distance = 100.f;
}


void Object_manager::draw() {
    for (Entity* entity : entities) {
        entity->draw(game);
    }
}


void Entity::draw(Game* game) {
    SDL_Rect rect = {0, 0, surface->w, surface->h};
    draw(game, rect);
}


void Entity::draw(Game* game, SDL_Rect& rect) {

    int height = 2 * game->width / 3;

    // from camera coords to film coords then to pixel coords
    int pixel_x = int(0.5 * game->width * (1.0 + camera_x / camera_y));

    // if the sprite is outside the camera field of view ignore the prite
    if (camera_y < 0.5f) {
        return;
    }

    //calculate size of the sprite on screen, using 'camera_y' instead of the real distance to avoid fisheye
    //this is the dimension in pixel
    int sprite_height = int(size * height / camera_y * 15.f / 14.f);
    int sprite_width = sprite_height * rect.w / rect.h;
    
    int y_offset = height / 2 - static_cast<int>((float) height * (size - 0.5) / camera_y * 15.f / 14.f);
    
    float texture_step_x = (float) rect.w / (float) sprite_width;
    float texture_step_y = (float) rect.h / (float) sprite_height;

    if ((2*pixel_x - sprite_width < game->width) && (2*pixel_x + sprite_width > game->width) && (camera_y < game->raycaster.rays_lenght[game->width/2]) && !transparent) {
        game->entities_manager.targeted_entity = this;
        game->entities_manager.targeted_entity_distance = camera_y; 
    }

    for (int i=0; i < sprite_width; i++) {

        int x = pixel_x - sprite_width / 2 + i;
        int tex_x = rect.x + i*texture_step_x;

        if ( x < 0 || x > game->width ) {
            continue;
        }
        if (camera_y > game->raycaster.rays_lenght[x]) {
            continue;
        }
        
        sprite_height = (sprite_height + y_offset >= height) ? height - y_offset - 1 : sprite_height;
        for (int y = y_offset<0 ? -y_offset : 0; y < sprite_height; y++) {
            int tex_y = rect.y + y*texture_step_y;
            Uint32 color = ((Uint32*) surface->pixels)[tex_x + tex_y*surface->w];
            if (!(color >> 24)) continue;
            game->scene_pixels[x + (y + y_offset)*game->width] = color;
        }
    }
}


Barrel::Barrel(float x, float y) : Entity(x, y, 1.f, 1.f, false) {
    surface = Object_manager::getSurface("ressources/entities/barrel.png");
}





FireBall::FireBall(float x, float y, float v_x, float v_y) : Entity(x, y, 0.35f, 1.f, true), v_x(v_x), v_y(v_y), cooldown(-1) {
    surface = Object_manager::getSurface("ressources/entities/fire_ball.png");
    if (!FireBall::explode) {
        FireBall::explode = Mix_LoadWAV("ressources/sounds/rocket_explosion.wav");
    }
}
    

bool FireBall::update(Game* game) {
    if (cooldown==0) {
        return true;
    }
    if (cooldown>0) {
        if (game->animation) cooldown--;
        return false;
    };

    float x = position_x + v_x*game->delta_time;
    float y = position_y + v_y*game->delta_time;
    if (game->map.collide(x, y)) {
        cooldown = 5;
        Mix_PlayChannel(-1, explode, 0);
        size = 1.f;
        return false;
    } else {
        position_x = x;
        position_y = y;
    }
    for (auto entity : game->entities_manager.entities) {
        if (!entity->transparent && ((*entity).position_x - position_x)*((*entity).position_x - position_x) + ((*entity).position_y - position_y)*((*entity).position_y - position_y) < entity->size*size) {
            entity->damage(20.f);
            cooldown = 5;
            Mix_PlayChannel(-1, explode, 0);
            size = 1.f;
            break;
        }
    }
    return false;
}

void FireBall::draw(Game* game) {
    if (cooldown==-1) {
        Entity::draw(game, balls_rects[0]);
    } else {
        Entity::draw(game, balls_rects[2+cooldown/2]);
    }
}


PlasmaBall::PlasmaBall(float x, float y, float v_x, float v_y) : Entity(x, y, 0.2f, 1.f, true), v_x(v_x), v_y(v_y), cooldown(-1) {
    surface = Object_manager::getSurface("ressources/entities/plasma_ball.png");
}
    

bool PlasmaBall::update(Game* game) {
    if (cooldown==0) {
        return true;
    }
    if (cooldown>0) {
        if (game->animation) cooldown--;
        return false;
    };

    float x = position_x + v_x*game->delta_time;
    float y = position_y + v_y*game->delta_time;
    if (game->map.collide(x, y)) {
        cooldown = 5;
        size = 1.f;
        return false;
    } else {
        position_x = x;
        position_y = y;
    }
    for (auto entity : game->entities_manager.entities) {
        if (!entity->transparent && ((*entity).position_x - position_x)*((*entity).position_x - position_x) + ((*entity).position_y - position_y)*((*entity).position_y - position_y) < entity->size*size) {
            entity->damage(10.f);
            cooldown = 5;
            size = 0.7f;
            break;
        }
    }
    return false;
}

void PlasmaBall::draw(Game* game) {
    if (cooldown==-1) {
        Entity::draw(game, balls_rects[0]);
    } else {
        Entity::draw(game, balls_rects[1+cooldown/2]);
    }
}


Ammos::Ammos(float x, float y, int quantity, int weapon_id) : Entity(x, y, 0.2f, 1, true), weapon_id(weapon_id), quantity(quantity) {
    surface = Object_manager::getSurface("ressources/entities/ammos.png");
}

void Ammos::draw(Game* game) {
    switch (weapon_id) {
    case 1:
        Entity::draw(game, Ammos::ammos_rects[0]);
        break;
    case 2:
        Entity::draw(game, Ammos::ammos_rects[1]);
        break;
    case 3:
        Entity::draw(game, Ammos::ammos_rects[2]);
        break;
    case 4:
        Entity::draw(game, Ammos::ammos_rects[3]);
        break;
    }
}

bool Ammos::update(Game* game) {
    if (game->player.weapons[weapon_id]->available && (camera_y*camera_y + camera_x*camera_x) < 0.9f) {
        game->player.weapons[weapon_id]->munitions += quantity;
        return true;
    }
    return false;
}




bool Enemy::update(Game* game) {

    // figure out if enemy has a direct view to the player, cast a ray from player to enemy
    float dist = hypotf(position_x - game->player.position_x, position_y - game->player.position_y);

    // direction step
    int step_x;
    int step_y;

    // x and y ray sides
    float x_ray_length = 0;
    float y_ray_length = 0;

    // ray directions
    float ray_dir_x = (position_x - game->player.position_x) / dist;
    float ray_dir_y = (position_y - game->player.position_y) / dist;

    // ray lengths per x,y step
    float x_ray_unit_length = std::abs(1.f / ray_dir_x);
    float y_ray_unit_length = std::abs(1.f / ray_dir_y);

    // player cell location
    int current_cell_x = static_cast<int>(game->player.position_x);
    int current_cell_y = static_cast<int>(game->player.position_y);

    if (ray_dir_x > 0) { // we look at west
        x_ray_length += x_ray_unit_length * (1.f - (game->player.position_x - current_cell_x));
        step_x = 1;
    } else if (ray_dir_x < 0) { // we look at east
        x_ray_length += x_ray_unit_length * (game->player.position_x - current_cell_x);
        step_x = -1;
    }
    if (ray_dir_y > 0) { // we look at south
        y_ray_length += y_ray_unit_length * (1.f - (game->player.position_y - current_cell_y));
        step_y = 1;
    } else if (ray_dir_y < 0) { // we look at north
        y_ray_length += y_ray_unit_length * (game->player.position_y - current_cell_y);
        step_y = -1;
    }

    char collision_side;
    for (int i=0; i<50; i++) {
        if (x_ray_length < y_ray_length) {
            x_ray_length += x_ray_unit_length;
            current_cell_x += step_x;
            collision_side = 'x';
        } else {
            y_ray_length += y_ray_unit_length;
            current_cell_y += step_y;
            collision_side = 'y';
        }
        if (game->map.collide(current_cell_x, current_cell_y)){
            break;
        }
    }
    if (collision_side == 'x') {
        direct_ray = (x_ray_length - x_ray_unit_length) > dist;
    } else {
        direct_ray = (y_ray_length - y_ray_unit_length) > dist;
    }

    if (game->animation) animation_cooldown++;

    float move_x = 0;
    float move_y = 0;
    switch (status) {
        case npc_status_t::WAIT:
            if (direct_ray) {
                status = npc_status_t::WALK;
            }
            break;
        case npc_status_t::WALK:
            if (direct_ray && dist < 5) {
                status = npc_status_t::SHOOT;
            } else if (direct_ray) {
                move_x = (game->player.position_x - position_x) / dist;
                move_y = (game->player.position_y - position_y) / dist;
            } else {
                int a = game->map.map[static_cast<int>(position_x)][static_cast<int>(position_y)].dir;
                if (a==0) {
                    status = npc_status_t::WAIT;
                    return false;
                }
                if (a%2) {
                    move_x = a-2;
                    move_y = 0;
                } else {
                    move_x = 0;
                    move_y = a-3;
                }
            }
            break;
        case npc_status_t::SHOOT:
            if (!direct_ray || dist > 8) {
                status = npc_status_t::WALK;
            } else if (game->animation) {
                game->player.damage(weapon_damage / 20);
            }
            break;
        case npc_status_t::PAIN:
            if (animation_cooldown >= 5) status = npc_status_t::WAIT;
            break;
        case npc_status_t::DIYING:
            if (animation_cooldown >= 9) {
                int x = rand()%64;
                int y = rand()%64;
                while (game->map.collide(x,y)) {
                    x = rand()%64;
                    y = rand()%64;
                }
                game->entities_manager.entities.push_back(new Soldier1(x+0.5, y+0.5));
                return true;
            }
            break;
    }
    

    move_x *= game->delta_time*velocity;
    move_y *= game->delta_time*velocity;

    if (game->map.collide(move_x+position_x, position_y)) {
        if (move_x>0) {
            move_x = floor((position_x+move_x)) - position_x - 0.01;
        } else if (move_x<0) {
            move_x = ceil((position_x+move_x)) - position_x + 0.01;
        }
    }
    if (game->map.collide(position_x, position_y+move_y)) {
        if (move_y>0) {
            move_y = floor((position_y+move_y)) - position_y - 0.01;
        } else if (move_y<0) {
            move_y = ceil((position_y+move_y)) - position_y + 0.01;
        }
    }

    position_x += move_x;
    position_y += move_y;

    return false;
}


Soldier1::Soldier1(float x, float y) : Enemy(x,y,0.7f,50.f,4.f) {
    surface = Object_manager::getSurface("ressources/entities/soldier_1.png");
    status = WAIT;
}

bool Soldier1::update(Game* game) {
    return Enemy::update(game);
}

void Soldier1::draw(Game* game) {
    int s = animation_cooldown;
    switch (status) {
    case npc_status_t::WAIT:
        Entity::draw(game, Soldier1::walk_front_rects[0]);
        break;
    
    case npc_status_t::WALK:
        s /= 3;
        s %= 4;
        Entity::draw(game, Soldier1::walk_front_rects[s]);
        break;
    
    case npc_status_t::SHOOT:
        s /= 3;
        s %= 2;
        Entity::draw(game, Soldier1::shoot_rects[s]);
        break;
    
    case npc_status_t::PAIN:
        Entity::draw(game, Soldier1::pain_rect[0]);
        break;
    
    case npc_status_t::DIYING:
        s /= 1;
        s %= 9;
        Entity::draw(game, Soldier1::death1[s]);
        break;
    
    default:
        break;
    }
    
};

void Soldier1::damage(float value) {
    if (status == npc_status_t::DIYING) return;
    if (value == 0) return;
    status = npc_status_t::PAIN;
    animation_cooldown = 0;
    health -= value;
    if (health <= 0) {
        death();
    }
}

void Soldier1::death() {
    status = npc_status_t::DIYING;
}
