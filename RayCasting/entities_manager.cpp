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
    entities.push_back(Entity(3.0,3.0));
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

    for (std::vector<Entity>::iterator entity = begin (entities); entity != end (entities); ++entity) {

        // from camera coords to film coords then to pixel coords
        float pixel_x = 0.5 * (float) WINDOW_WIDTH * (1.0 + entity->camera_x / entity->camera_y);


        // if the sprite is outside the camera field of view ignore the prite
        if (pixel_x < 0 || pixel_x > WINDOW_WIDTH || entity->camera_y < 0) {
            continue;
        }

        // if the entity is behind a wall
        if (entity->camera_y > game->raycaster.rays_lenght[(unsigned int) pixel_x]) {
            continue;
        }

        //calculate height of the sprite on screen, using 'camera_y' instead of the real distance to avoid fisheye
        float sprite_height = 0.5 * std::abs((float) WINDOW_HEIGHT / entity->camera_y) / (float) barrel_texture.getSize().y;
        float sprite_width = 0.5 * std::abs((float) WINDOW_HEIGHT / entity->camera_y) / (float) barrel_texture.getSize().x;

        sprite_barrel.setScale(sf::Vector2f(sprite_width, sprite_height));
        sprite_barrel.setPosition(pixel_x - 0.25 * std::abs((float) WINDOW_HEIGHT / entity->camera_y) , WINDOW_HEIGHT / 2.0 - 0.5 * sprite_height);
        game->window.draw(sprite_barrel);

    }

}


Entity::Entity(float x, float y) : position_x(x), position_y(y) {}