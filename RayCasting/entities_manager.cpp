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


std::array<SDL_Rect, 4> Soldier2::walk_front_rects = {
    SDL_Rect{11, 5, 44, 55},
    SDL_Rect{61, 5, 44, 55},
    SDL_Rect{109, 5, 44, 55},
    SDL_Rect{155, 5, 44, 55}
};
std::array<SDL_Rect, 2> Soldier2::shoot_rects = {
    SDL_Rect{211, 5, 27, 55},
    SDL_Rect{267, 5, 27, 55}
};
std::array<SDL_Rect, 1> Soldier2::pain_rect = {
    SDL_Rect{318, 5, 44, 55}
};
std::array<SDL_Rect, 9> Soldier2::death1 = {
    SDL_Rect{1, 250, 40, 61},
    SDL_Rect{45, 250, 42, 61},
    SDL_Rect{91, 250, 48, 61},
    SDL_Rect{143, 250, 52, 61},
    SDL_Rect{198, 250, 55, 61},
    SDL_Rect{256, 250, 57, 61},
    SDL_Rect{317, 250, 56, 61},
    SDL_Rect{376, 250, 57, 61},
    SDL_Rect{436, 250, 57, 61}
};


std::array<SDL_Rect, 4> Soldier3::walk_front_rects = {
    SDL_Rect{5, 12, 45, 56},
    SDL_Rect{61, 12, 45, 56},
    SDL_Rect{113, 12, 45, 56},
    SDL_Rect{163, 12, 45, 56}
};
std::array<SDL_Rect, 2> Soldier3::shoot_rects = {
    SDL_Rect{221, 12, 26, 56},
    SDL_Rect{279, 12, 26, 56}
};
std::array<SDL_Rect, 1> Soldier3::pain_rect = {
    SDL_Rect{327, 5, 40, 56}
};
std::array<SDL_Rect, 9> Soldier3::death1 = {
    SDL_Rect{5, 256, 38, 60},
    SDL_Rect{46, 256, 44, 60},
    SDL_Rect{93, 256, 48, 60},
    SDL_Rect{144, 256, 53, 60},
    SDL_Rect{200, 256, 55, 60},
    SDL_Rect{258, 256, 57, 60},
    SDL_Rect{318, 256, 57, 60},
    SDL_Rect{379, 256, 56, 60},
    SDL_Rect{438, 256, 57, 60}
};


std::array<SDL_Rect, 4> CyberDemon::walk_front_rects = {
    SDL_Rect{5, 32, 90, 110},
    SDL_Rect{172, 32, 90, 110},
    SDL_Rect{324, 32, 90, 110},
    SDL_Rect{488, 32, 90, 110}
};
std::array<SDL_Rect, 2> CyberDemon::shoot_rects = {
    SDL_Rect{642, 32, 100, 110},
    SDL_Rect{838, 32, 100, 110}
};
std::array<SDL_Rect, 1> CyberDemon::pain_rect = {
    SDL_Rect{1011, 32, 124, 110}
};
std::array<SDL_Rect, 9> CyberDemon::death1 = {
    SDL_Rect{10, 424, 122, 134},
    SDL_Rect{175, 424, 111, 134},
    SDL_Rect{330, 424, 100, 134},
    SDL_Rect{473, 424, 112, 134},
    SDL_Rect{629, 424, 125, 134},
    SDL_Rect{798, 424, 135, 134},
    SDL_Rect{986, 424, 141, 134},
    SDL_Rect{1170, 424, 139, 134},
    SDL_Rect{1353, 424, 128, 134}
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

std::array<SDL_Rect, 5> Weapons::weapons_rects = {
    SDL_Rect{0, 0, 63, 12},
    SDL_Rect{64, 0, 54, 16},
    SDL_Rect{119, 0, 62, 16},
    SDL_Rect{182, 0, 54, 16},
    SDL_Rect{237, 0, 62, 24}
};

std::array<SDL_Rect, 3> HealthPack::health_pack_rects = {
    SDL_Rect{0, 0, 14, 15},
    SDL_Rect{44, 0, 28, 19},
    SDL_Rect{15, 0, 28, 19}
};

std::array<SDL_Rect, 6> Armor::armor_rects = {
    SDL_Rect{0, 0, 16, 15},
    SDL_Rect{19, 0, 16, 15},
    SDL_Rect{38, 0, 31, 17},
    SDL_Rect{72, 0, 31, 17},
    SDL_Rect{110, 0, 31, 17},
    SDL_Rect{144, 0, 31, 17}
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
    case 0:
        entities.push_back(new Barrel(5,5));
        entities.push_back(new CyberDemon(4.5,44.0));

        entities.push_back(new Ammos(12,10,50,1));
        entities.push_back(new Ammos(12,12,50,2));
        entities.push_back(new Ammos(12,14,50,3));
        entities.push_back(new Ammos(12,16,50,4));

        entities.push_back(new Weapons(12,18,100,1));
        entities.push_back(new Weapons(12,20,100,2));
        entities.push_back(new Weapons(12,22,100,3));
        entities.push_back(new Weapons(12,24,100,4));
        entities.push_back(new Weapons(12,26,100,5));

        entities.push_back(new HealthPack(12,28,0));
        entities.push_back(new HealthPack(12,30,1));
        entities.push_back(new HealthPack(12,32,2));

        entities.push_back(new Armor(10,10,0));
        entities.push_back(new Armor(10,12,1));
        entities.push_back(new Armor(10,14,2));
        break;

    case 1:
        entities.push_back(new Weapons(12,40.5,20,1));
        entities.push_back(new Armor(12,43.5,0));
        entities.push_back(new Soldier1(1.5,53.5));

        entities.push_back(new Soldier1(3,62));
        entities.push_back(new Ammos(5,61,50,1));

        entities.push_back(new Soldier1(10,62));
        entities.push_back(new Soldier1(15,61));

        entities.push_back(new Soldier1(25,55));
        entities.push_back(new Soldier1(25,60));
        entities.push_back(new Soldier2(28,58));
        entities.push_back(new HealthPack(29,32,2));
        entities.push_back(new Weapons(28,58,100,2));

        entities.push_back(new Soldier1(10,47));
        entities.push_back(new Soldier1(25.5,45.5));
        entities.push_back(new Soldier1(25.5,41.5));

        // right

        entities.push_back(new Soldier1(49.5,35.5));
        entities.push_back(new Soldier1(49.5,36.5));
        entities.push_back(new Soldier1(51.5,36));
        entities.push_back(new Soldier2(45.5,32.5));
        entities.push_back(new Ammos(45.5,32.5,100,2));
        entities.push_back(new Ammos(51.5,36,20,1));

        entities.push_back(new Soldier1(38.5,43.5));
        entities.push_back(new Soldier1(42.5,48.5));
        entities.push_back(new Soldier1(41.5,53.5));
        entities.push_back(new Soldier2(44.5,59.5));

        entities.push_back(new Soldier2(32.5,56.5));
        entities.push_back(new Soldier2(36.5,53.5));
        entities.push_back(new Soldier2(32.5,53.5));
        entities.push_back(new Soldier2(36.5,56.5));
        entities.push_back(new Weapons(20,51,30,3));

        entities.push_back(new Ammos(50.5,58.5,30,3));
        entities.push_back(new Armor(56.5,58.5,1));
        entities.push_back(new Soldier2(58.5,61.5));
        entities.push_back(new Soldier2(59.5,62.5));

        entities.push_back(new Soldier1(50.5,50.5));
        entities.push_back(new Soldier1(50.5,48.5));
        entities.push_back(new Soldier3(57.5,38.5));
        entities.push_back(new Ammos(57.5,38.5,200,2));
        entities.push_back(new Weapons(58,31,0,5));
        entities.push_back(new HealthPack(51,23,1));

        entities.push_back(new Soldier3(34.5,25.5));
        entities.push_back(new Soldier2(32.5,21.5));
        entities.push_back(new Soldier2(32.5,19.5));
        entities.push_back(new Armor(31.5,20.5,2));

        entities.push_back(new Soldier2(41.5,13.5));
        entities.push_back(new Ammos(41.5,13.5,50,2));
        entities.push_back(new Soldier2(41.5,18.5));
        entities.push_back(new Ammos(41.5,18.5,20,3));

        // left

        entities.push_back(new Soldier1(15.5,35.5));
        entities.push_back(new Soldier1(15.5,36.5));
        entities.push_back(new Soldier1(15.5,36));
        entities.push_back(new Soldier2(16,33.5));
        entities.push_back(new Ammos(17.5,33.5,100,2));
        entities.push_back(new Ammos(17.5,34.5,20,1));

        entities.push_back(new Soldier1(10.5,26.5));
        entities.push_back(new Soldier1(14.5,28.5));
        entities.push_back(new Soldier1(16.5,26.5));
        entities.push_back(new Soldier2(23.5,29.5));

        entities.push_back(new Soldier2(27,22));
        entities.push_back(new Soldier2(28,22));
        entities.push_back(new Soldier2(20,20.5));
        entities.push_back(new Soldier2(16,21));
        entities.push_back(new Weapons(28.5,31.5,30,3));

        entities.push_back(new Ammos(8.5,22.5,30,3));
        entities.push_back(new Armor(2.5,24,1));
        entities.push_back(new Soldier2(1.5,17.5));
        entities.push_back(new Soldier2(3,12));

        entities.push_back(new Soldier1(12.5,7.5));
        entities.push_back(new Soldier1(16.5,7.5));
        entities.push_back(new Soldier3(14.5,10.5));
        entities.push_back(new Ammos(10.5,9.5,200,2));
        entities.push_back(new Ammos(12.5,5.5,50,1));
        entities.push_back(new Weapons(16.5,10.5,40,4));
        entities.push_back(new HealthPack(14.5,10.5,2));

        entities.push_back(new Soldier3(26.5,4.5));
        entities.push_back(new Soldier2(34.5,4.5));
        entities.push_back(new Soldier2(32.5,8.5));
        entities.push_back(new Ammos(34.5,1.5,30,4));
        entities.push_back(new Armor(32.5,5.5,2));
        entities.push_back(new HealthPack(13,16,0));

        entities.push_back(new Soldier2(25.5,17.5));
        entities.push_back(new Soldier2(41.5,18.5));
        entities.push_back(new Ammos(34.5,12.5,20,3));

        entities.push_back(new Soldier1(59.3,3.5));
        entities.push_back(new Soldier1(60.5,4.5));
        entities.push_back(new Soldier2(61.5,5));

        entities.push_back(new HealthPack(40,1,1));
        entities.push_back(new Ammos(46.5,1.5,20,1));


        // end

        entities.push_back(new HealthPack(61.5,10.5,2));
        entities.push_back(new CyberDemon(45.5,10.5));
        entities.push_back(new Soldier2(51.5,7.5));
        entities.push_back(new Soldier3(52,9));
        entities.push_back(new Soldier2(51.5,13.5));
        entities.push_back(new Soldier3(52,12));

        break;
    
    case 2:
        // corridor 1
        entities.push_back(new HealthPack(9.5,34.5,1));
        entities.push_back(new Soldier1(2,31));
        entities.push_back(new Weapons(1.5,25.5,20,1));
        entities.push_back(new Ammos(5.5,25.5,20,1));
        entities.push_back(new Soldier1(6,16));
        entities.push_back(new Soldier1(18.5,5.5));
        entities.push_back(new Soldier1(19.5,6.5));
        entities.push_back(new Soldier1(22.5,7.5));
        entities.push_back(new Soldier2(39.5,6.5));
        entities.push_back(new Soldier1(41.5,9.5));
        entities.push_back(new Soldier1(51.5,8.5));
        entities.push_back(new Soldier1(51.5,9.5));
        entities.push_back(new Soldier1(48.5,9));
        entities.push_back(new Ammos(52.5,5.5,10,1));
        entities.push_back(new Soldier2(59.5,16.5));
        entities.push_back(new Soldier1(61,20));
        entities.push_back(new Weapons(55.5,19.5,100,2));
        entities.push_back(new Soldier1(58,34));
        entities.push_back(new Soldier1(60.5,34.5));
        entities.push_back(new Soldier1(60.5,31.5));
        entities.push_back(new Soldier1(58.5,27.5));
        entities.push_back(new Ammos(60.5,35.5,50,2));

        // corridor 2
        entities.push_back(new Ammos(57.5,39.5,20,1));
        entities.push_back(new Soldier1(62.5,38.5));
        entities.push_back(new Soldier1(58.5,45.5));
        entities.push_back(new Soldier1(57.5,44.5));
        entities.push_back(new Soldier1(56.5,43.5));
        entities.push_back(new Soldier2(60.5,58.5));
        entities.push_back(new Soldier1(62.5,58.5));
        entities.push_back(new Ammos(62.5,60.5,70,2));
        entities.push_back(new HealthPack(49.5,55.5,1));
        entities.push_back(new Soldier2(47.5,60.5));
        entities.push_back(new Soldier1(36.5,60.5));
        entities.push_back(new Soldier1(37.5,61.5));
        entities.push_back(new Soldier1(37.5,59.5));
        entities.push_back(new Armor(30.5,57.5,0));
        entities.push_back(new Ammos(29.5,61.5,70,2));
        entities.push_back(new Ammos(25.5,62.5,30,1));
        entities.push_back(new Soldier3(2.5,45.5));
        entities.push_back(new Soldier3(1.5,42.5));
        entities.push_back(new Weapons(12.5,38.5,30,3));

        // bunker 1
        entities.push_back(new HealthPack(11.5,47.5,2));
        entities.push_back(new Soldier2(12.5,42.5));
        entities.push_back(new Armor(17,39,1));
        entities.push_back(new Ammos(11,52,70,2));
        entities.push_back(new Soldier2(14.5,55.5));
        entities.push_back(new Soldier2(17.5,57.5));
        entities.push_back(new Soldier1(24.5,58.5));
        entities.push_back(new Soldier1(24.5,55.5));
        entities.push_back(new HealthPack(27.5,52.5,0));
        entities.push_back(new Ammos(24.5,47.5,30,1));
        entities.push_back(new Soldier3(21.5,40.5));
        entities.push_back(new Soldier1(28.5,40.5));
        entities.push_back(new Ammos(30.5,36.5,20,3));
        entities.push_back(new Ammos(37.5,38.5,70,2));
        entities.push_back(new Soldier1(24.5,44.5));
        entities.push_back(new Soldier1(25.5,45.5));
        entities.push_back(new Soldier2(36.5,50.5));
        entities.push_back(new Soldier2(40.5,48.5));
        entities.push_back(new HealthPack(33.5,53.5,1));
        entities.push_back(new Soldier3(45.5,49.5));
        entities.push_back(new Soldier1(50.5,49.5));
        entities.push_back(new Soldier1(48.5,48.5));
        entities.push_back(new Weapons(57.5,51.5,50,4));
        entities.push_back(new Soldier1(41.5,45.5));
        entities.push_back(new Soldier1(53.5,45.5));
        entities.push_back(new HealthPack(37,42,1));
        entities.push_back(new Ammos(41.5,41.5,30,1));
        entities.push_back(new Ammos(40.5,39.5,70,2));
        entities.push_back(new Ammos(47.5,41.5,30,3));

        // midle corridor
        entities.push_back(new Soldier2(19.5,33.5));
        entities.push_back(new Soldier2(18.5,34.5));
        entities.push_back(new Soldier2(17.5,35.5));
        entities.push_back(new Soldier2(15.5,34.5));
        entities.push_back(new Ammos(11.5,34.5,70,2));

        // bunker 2
        entities.push_back(new Soldier1(8.5,23.5));
        entities.push_back(new Soldier1(9.5,19.5));
        entities.push_back(new Soldier1(13.5,19.5));
        entities.push_back(new Armor(16.5,20.2,2));
        entities.push_back(new Soldier3(11.5,11.5));
        entities.push_back(new Ammos(17.5,9.5,70,2));
        entities.push_back(new Soldier1(22.5,17.5));
        entities.push_back(new Soldier1(22.5,15.5));
        entities.push_back(new Ammos(28.5,15.5,30,1));
        entities.push_back(new Weapons(21,13,0,5));
        entities.push_back(new Soldier2(23,20));
        entities.push_back(new Soldier2(20.5,19.5));
        entities.push_back(new Soldier2(33.5,23.5));
        entities.push_back(new Soldier1(37.5,18.5));
        entities.push_back(new HealthPack(34.5,9.5,1));
        entities.push_back(new Ammos(35.5,9.5,70,2));
        entities.push_back(new Soldier3(44.5,17.5));
        entities.push_back(new Soldier2(52.5,13.5));
        entities.push_back(new Soldier1(56.5,9.5));
        entities.push_back(new Ammos(56.5,31.5,30,4));
        entities.push_back(new Soldier2(47.5,21.5));
        entities.push_back(new Ammos(40.5,20.5,200,2));
        entities.push_back(new HealthPack(39.5,26.5,2));

        // final room
        entities.push_back(new CyberDemon(19.5,28.5));
        entities.push_back(new Soldier2(16.5,27.5));
        entities.push_back(new Soldier2(16.5,28.5));
        entities.push_back(new Soldier3(33.5,30.5));

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




Weapons::Weapons(float x, float y, int munitions, int weapon_id) : Entity(x, y, 0.2f, 1, true), weapon_id(weapon_id), munitions(munitions) {
    surface = Object_manager::getSurface("ressources/entities/weapons.png");
}

void Weapons::draw(Game* game) {
    Entity::draw(game, Weapons::weapons_rects[weapon_id-1]);
}

bool Weapons::update(Game* game) {
    if ((camera_y*camera_y + camera_x*camera_x) < 0.9f) {
        game->player.weapons[weapon_id]->munitions += munitions;
        game->player.weapons[weapon_id]->available = true;
        return true;
    }
    return false;
}


HealthPack::HealthPack(float x, float y, int pack_id) : Entity(x, y, 0.2f, 1, true), pack_id(pack_id) {
    surface = Object_manager::getSurface("ressources/entities/health_pack.png");
}

void HealthPack::draw(Game* game) {
    Entity::draw(game, HealthPack::health_pack_rects[pack_id]);
}

bool HealthPack::update(Game* game) {
    if ((camera_y*camera_y + camera_x*camera_x) < 0.9f) {
        game->player.blood_level = 0;
        switch (pack_id) {
        case 0:
            game->player.health += 10;
            if (game->player.health > 100) game->player.health = 100;
            break;
        case 1:
            game->player.health += 25;
            if (game->player.health > 100) game->player.health = 100;
            break;
        case 2:
            game->player.health = 100;
            break;
        }
        return true;
    }
    return false;
}



Armor::Armor(float x, float y, int armor_id) : Entity(x, y, 0.2f, 1, true), armor_id(armor_id) {
    surface = Object_manager::getSurface("ressources/entities/items.png");
}

void Armor::draw(Game* game) {
    static int s = 0;
    if (game->animation) s++;
    Entity::draw(game, Armor::armor_rects[2*armor_id + s/5%2]);
}

bool Armor::update(Game* game) {
    if ((camera_y*camera_y + camera_x*camera_x) < 0.9f) {
        game->player.armors[armor_id] = true;
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
            return false;
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
            return false;
        case npc_status_t::PAIN:
            if (animation_cooldown >= 5) status = npc_status_t::WAIT;
            return false;
        case npc_status_t::DIYING:
            if (animation_cooldown >= death_cooldown) {
                game->player.frag += 1;
                if (final_enemy) game->player.game_won = 80;
                if (rand()%3 == 0)
                    game->entities_manager.entities.push_back(new HealthPack(position_x, position_y, 0));
                return true;
            }
            return false;
    }
    
    move_x *= game->delta_time*velocity;
    move_y *= game->delta_time*velocity;

    position_x += move_x;
    if (game->map.collide(position_x + 0.2f, position_y))
        position_x = floor(position_x + 0.2f) - 0.2f;

    if (game->map.collide(position_x - 0.2f, position_y))
        position_x = ceil(position_x - 0.2f) + 0.2f;


    position_y += move_y;
    if (game->map.collide(position_x, position_y + 0.2f))
        position_y = floor(position_y + 0.2f) - 0.2f;
    
    if (game->map.collide(position_x, position_y - 0.2f))
        position_y = ceil(position_y - 0.2f) + 0.2f;

    return false;
}

void Enemy::death() {
    status = npc_status_t::DIYING;
    transparent = true;
}

void Enemy::damage(float value) {
    if (status == npc_status_t::DIYING) return;
    if (value == 0) return;
    status = npc_status_t::PAIN;
    animation_cooldown = 0;
    health -= value;
    if (health <= 0) {
        death();
    }
}




Soldier1::Soldier1(float x, float y) : Enemy(x,y,0.7f,80.f,10.f,false) {
    surface = Object_manager::getSurface("ressources/entities/soldier_1.png");
    status = WAIT;
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




Soldier2::Soldier2(float x, float y) : Enemy(x,y,0.7f,150.f,20.f,false) {
    surface = Object_manager::getSurface("ressources/entities/soldier_2.png");
    status = WAIT;
}

void Soldier2::draw(Game* game) {
    int s = animation_cooldown;
    switch (status) {
    case npc_status_t::WAIT:
        Entity::draw(game, Soldier2::walk_front_rects[0]);
        break;
    
    case npc_status_t::WALK:
        s /= 3;
        s %= 4;
        Entity::draw(game, Soldier2::walk_front_rects[s]);
        break;
    
    case npc_status_t::SHOOT:
        s /= 3;
        s %= 2;
        Entity::draw(game, Soldier2::shoot_rects[s]);
        break;
    
    case npc_status_t::PAIN:
        Entity::draw(game, Soldier2::pain_rect[0]);
        break;
    
    case npc_status_t::DIYING:
        s /= 1;
        s %= 9;
        Entity::draw(game, Soldier2::death1[s]);
        break;
    
    default:
        break;
    }
    
};


Soldier3::Soldier3(float x, float y) : Enemy(x,y,0.7f,200.f,40.f,false) {
    surface = Object_manager::getSurface("ressources/entities/soldier_3.png");
    status = WAIT;
}

void Soldier3::draw(Game* game) {
    int s = animation_cooldown;
    switch (status) {
    case npc_status_t::WAIT:
        Entity::draw(game, Soldier3::walk_front_rects[0]);
        break;
    
    case npc_status_t::WALK:
        s /= 3;
        s %= 4;
        Entity::draw(game, Soldier3::walk_front_rects[s]);
        break;
    
    case npc_status_t::SHOOT:
        s /= 3;
        s %= 2;
        Entity::draw(game, Soldier3::shoot_rects[s]);
        break;
    
    case npc_status_t::PAIN:
        Entity::draw(game, Soldier3::pain_rect[0]);
        break;
    
    case npc_status_t::DIYING:
        s /= 1;
        s %= 9;
        Entity::draw(game, Soldier3::death1[s]);
        break;
    
    default:
        break;
    }
    
};



CyberDemon::CyberDemon(float x, float y) : Enemy(x,y,0.7f,500.f,50.f,true) {
    surface = Object_manager::getSurface("ressources/entities/cyberDemon.png");
    status = WAIT;
    death_cooldown = 27;
}

void CyberDemon::draw(Game* game) {
    int s = animation_cooldown;
    switch (status) {
    case npc_status_t::WAIT:
        Entity::draw(game, CyberDemon::walk_front_rects[0]);
        break;
    
    case npc_status_t::WALK:
        s /= 6;
        s %= 4;
        Entity::draw(game, CyberDemon::walk_front_rects[s]);
        break;
    
    case npc_status_t::SHOOT:
        s /= 3;
        s %= 2;
        Entity::draw(game, CyberDemon::shoot_rects[s]);
        break;
    
    case npc_status_t::PAIN:
        Entity::draw(game, CyberDemon::pain_rect[0]);
        break;
    
    case npc_status_t::DIYING:
        s /= 3;
        s %= 9;
        Entity::draw(game, CyberDemon::death1[s]);
        break;
    
    default:
        break;
    }
    
};