#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>


#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"


// initializatons

std::map<std::string, SDL_Surface*> Object_manager::surfaces;

SDL_Surface* Object_manager::getSurface(std::string name) {
    if(surfaces.find(name) != surfaces.end()){
        return surfaces[name];
    } else {
        std::cout << "load new texture\n";
        SDL_Surface* new_surface = IMG_Load(name.c_str());
        SDL_Surface* new_formated_surface = new SDL_Surface();
        new_formated_surface = SDL_ConvertSurfaceFormat(new_surface, SDL_PIXELFORMAT_ABGR8888, 0);
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





Object_manager::Object_manager(Game* game) : game(game) {
    // barrel_texture.loadFromFile("ressources/barrel.png");
    entities.push_back(new Soldier1(10.5,5.5));
    entities.push_back(new Barrel(4.0,3.0));
}

void Object_manager::update() {
    // transform sprite with the inverse camera matrix to get their coordinate in the camera space

    // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
    // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
    // [ planeY   dirY ]                                          [ -planeY  planeX ]

    float inv_det = 1.0 / (game->player.plane_x * game->player.dir_y - game->player.dir_x * game->player.plane_y);

    for (Entity* entity : entities) {
        float sprite_x = entity->position_x - game->player.position_x;
        float sprite_y = entity->position_y - game->player.position_y;

        // coordinate in the camera space: from world coords to camera coords
        entity->camera_x = inv_det * (game->player.dir_y * sprite_x - game->player.dir_x * sprite_y);
        entity->camera_y = inv_det * (-game->player.plane_y * sprite_x + game->player.plane_x * sprite_y); // this is actually the depth inside the screen
        entity->update(game);
    }

    // sort the entities to display the furthest before the closest
    std::sort(entities.begin(), entities.end(), [] (Entity* a, Entity* b) {return a->camera_y > b->camera_y;});
}


void Object_manager::draw() {
    for (Entity* entity : entities) {
        entity->draw(game);
    }
}







void Entity::update(Game* game) {}


void Entity::draw(Game* game) {
    SDL_Rect rect = {0, 0, surface->w, surface->h};
    draw(game, rect, 1);
}


void Entity::draw(Game* game, SDL_Rect& rect, float size) {

    // from camera coords to film coords then to pixel coords
    int pixel_x = int(0.5 * WINDOW_WIDTH * (1.0 + camera_x / camera_y));

    // if the sprite is outside the camera field of view ignore the prite
    if (camera_y < 0.5f) {
        return;
    }

    //calculate size of the sprite on screen, using 'camera_y' instead of the real distance to avoid fisheye
    //this is the dimension in pixel
    int sprite_height = size * int(WINDOW_HEIGHT / camera_y);
    int sprite_width = sprite_height * rect.w / rect.h;
    
    int y_offset = WINDOW_HEIGHT / 2 - static_cast<int>((float) WINDOW_WIDTH * (size - 0.5) / (1.7f * camera_y));
    
    float texture_step_x = (float) rect.w / (float) sprite_width;
    float texture_step_y = (float) rect.h / (float) sprite_height;

    for (int i=0; i < sprite_width; i++) {

        int x = pixel_x - sprite_width / 2 + i;
        int tex_x = rect.x + i*texture_step_x;

        if ( x < 0 || x > WINDOW_WIDTH ) {
            continue;
        }
        if (camera_y > game->raycaster.rays_lenght[x]) {
            continue;
        }
        
        for (int y=0; y<sprite_height; y++) {
            if (y + y_offset >= WINDOW_HEIGHT) break;
            if (y + y_offset < 0) continue;
            int tex_y = rect.y + y*texture_step_y;
            Uint32 color = ((Uint32*) surface->pixels)[tex_x + tex_y*surface->w];
            if (!(color >> 24)) continue;
            game->scene_pixels[x + (y + y_offset)*WINDOW_WIDTH] = color;
        }
    }
}


Barrel::Barrel(float x, float y) : Entity(x,y) {
    surface = Object_manager::getSurface("ressources/barrel.png");
}







void Enemy::update(Game* game) {

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

    float move_x, move_y;
    if (direct_ray) {
        if (dist < 5) {
            status = SHOOT;
            return;
        }
        if (dist < 8 && status == SHOOT) {
            return;
        }
        status = WALK;
        move_x = (game->player.position_x - position_x) / dist;
        move_y = (game->player.position_y - position_y) / dist;

    } else {
        status = WALK;
        int a = game->map.map[static_cast<int>(position_x)][static_cast<int>(position_y)].dir;
        if (a==0) {
            return;
        }
        if (a%2) {
            move_x = a-2;
            move_y = 0;
        } else {
            move_x = 0;
            move_y = a-3;
        }
    }

    move_x *= game->delta_time;
    move_y *= game->delta_time;

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
}


Soldier1::Soldier1(float x, float y) : Enemy(x,y) {
    surface = Object_manager::getSurface("ressources/soldier_1.png");
    status = WAIT;
}

void Soldier1::update(Game* game) {
    Enemy::update(game);
}

void Soldier1::draw(Game* game) {
    static int s = 0;
    if (game->animation) {
        s++;
    }
    switch (status) {
    case WAIT:
        s %= 4;
        Entity::draw(game, Soldier1::walk_front_rects[s], 0.7);
        break;
    
    case WALK:
        s %= 4;
        Entity::draw(game, Soldier1::walk_front_rects[s], 0.7);
        break;
    
    case SHOOT:
        s %= 2;
        Entity::draw(game, Soldier1::shoot_rects[s], 0.7);
        break;
    
    default:
        break;
    }
    
};