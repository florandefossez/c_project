#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <iostream>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"

Object_manager::Object_manager(Game* game) : game(game) {
    barrel_texture.loadFromFile("ressources/barrel.png");
    sprite_barrel.setTexture(barrel_texture);
    entities.push_back(Entity(10.5,5.5));
    entities.push_back(Entity(4.0,3.0));
}


void Object_manager::update() {
    // transform sprite with the inverse camera matrix to get their coordinate in the camera space

    // [ planeX   dirX ] -1                                       [ dirY      -dirX ]
    // [               ]       =  1/(planeX*dirY-dirX*planeY) *   [                 ]
    // [ planeY   dirY ]                                          [ -planeY  planeX ]

    float inv_det = 1.0 / (game->player.plane_x * game->player.dir_y - game->player.dir_x * game->player.plane_y);

    for (std::vector<Entity>::iterator entity = begin(entities); entity != end(entities); ++entity) {
        float sprite_x = entity->position_x - game->player.position_x;
        float sprite_y = entity->position_y - game->player.position_y;

        // coordinate in the camera space: from world coords to camera coords
        entity->camera_x = inv_det * (game->player.dir_y * sprite_x - game->player.dir_x * sprite_y);
        entity->camera_y = inv_det * (-game->player.plane_y * sprite_x + game->player.plane_x * sprite_y); // this is actually the depth inside the screen

    }

    // sort the entities to display the furthest before the closest
    std::sort(entities.begin(), entities.end(), [] (Entity a, Entity b) {return a.camera_y > b.camera_y;});
}


void Object_manager::draw() {

    sf::Sprite strip;

    for (std::vector<Entity>::iterator entity = begin(entities); entity != end(entities); ++entity) {

        // from camera coords to film coords then to pixel coords
        int pixel_x = int(0.5 * WINDOW_WIDTH * (1.0 + entity->camera_x / entity->camera_y));

        // if the sprite is outside the camera field of view ignore the prite
        if (entity->camera_y < 0.5f) {
            continue;
        }

        //calculate size of the sprite on screen, using 'camera_y' instead of the real distance to avoid fisheye
        //this is the dimension in pixel
        int sprite_size = int(WINDOW_HEIGHT / entity->camera_y);
        
        strip.setTexture(barrel_texture);
        strip.setPosition(
            pixel_x - sprite_size / 2,
            -sprite_size/2 + WINDOW_HEIGHT / 2
        );
        
        strip.setScale(sf::Vector2f(1, (float) sprite_size / (float) barrel_texture.getSize().y));

        for (int i=0; i < sprite_size; i++) {

            int x = pixel_x - sprite_size / 2 + i;

            if ( x < 0 || x > WINDOW_WIDTH ) {
                strip.move(1,0);
                continue;
            }
            if (entity->camera_y > game->raycaster.rays_lenght[x]) {
                strip.move(1,0);
                continue;
            }
            
            strip.setTextureRect(sf::IntRect(
                i * barrel_texture.getSize().x / sprite_size,
                0,
                1,
                barrel_texture.getSize().y
            ));
            
            game->window.draw(strip);
            strip.move(1,0);
        }
    }
}


Entity::Entity(float x, float y) : position_x(x), position_y(y) {}