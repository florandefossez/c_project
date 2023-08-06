#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <iostream>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"


// initializatons

std::map<std::string, sf::Texture*> Object_manager::textures;

sf::Texture* Object_manager::getTexture(std::string name) {
    if(textures.find(name) != textures.end()){
        return textures[name];
    } else {
        std::cout << "load new texture\n";
        sf::Texture* texture = new sf::Texture();
        texture->loadFromFile(name);
        textures[name] = texture;
        return textures[name];
    }
}

std::array<sf::IntRect, 4> Soldier1::walk_front_rects = {
    sf::IntRect(5, 12, 43, 55),
    sf::IntRect(54, 12, 43, 55),
    sf::IntRect(103, 12, 43, 55),
    sf::IntRect(151, 12, 43, 55)
};

std::array<sf::IntRect, 2> Soldier1::shoot_rects = {
    sf::IntRect(204, 14, 32, 55),
    sf::IntRect(256, 14, 32, 55)
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
    sf::IntRect rect = sf::IntRect(0, 0, texture->getSize().x, texture->getSize().y);
    draw(game, rect, 1);
}


void Entity::draw(Game* game, sf::IntRect& rect, float size) {
    sf::Sprite strip;

    // from camera coords to film coords then to pixel coords
    int pixel_x = int(0.5 * WINDOW_WIDTH * (1.0 + camera_x / camera_y));

    // if the sprite is outside the camera field of view ignore the prite
    if (camera_y < 0.5f) {
        return;
    }

    //calculate size of the sprite on screen, using 'camera_y' instead of the real distance to avoid fisheye
    //this is the dimension in pixel
    int sprite_height = size * int(WINDOW_HEIGHT / camera_y);
    int sprite_width = sprite_height * rect.width / rect.height;
    
    strip.setTexture(*texture);
    strip.setPosition(
        pixel_x - sprite_width / 2,
        (float) WINDOW_HEIGHT / 2.f - (float) WINDOW_WIDTH * (size - 0.5) / (1.7f * camera_y)
    );
    
    strip.setScale(sf::Vector2f(1, (float) sprite_height / (float) rect.height));

    for (int i=0; i < sprite_width; i++) {

        int x = pixel_x - sprite_width / 2 + i;

        if ( x < 0 || x > WINDOW_WIDTH ) {
            strip.move(1,0);
            continue;
        }
        if (camera_y > game->raycaster.rays_lenght[x]) {
            strip.move(1,0);
            continue;
        }
        
        strip.setTextureRect(sf::IntRect(
            i * rect.width / sprite_width + rect.left,
            rect.top,
            1,
            rect.height
        ));
        
        game->window.draw(strip);
        strip.move(1,0);
    }
}


Barrel::Barrel(float x, float y) : Entity(x,y) {
    texture = Object_manager::getTexture("ressources/barrel.png");
}







void Enemy::update(Game* game) {

    float dist = hypotf(position_x - game->player.position_x, position_y - game->player.position_y);
    if (dist < 1.5) {status = SHOOT; return;}
    if (dist < 3 && status == SHOOT) return;
    status = WALK;

    float move_x = (game->player.position_x - position_x) / dist;
    float move_y = (game->player.position_y - position_y) / dist;

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


Soldier1::Soldier1(float x, float y) : Enemy(x,y), state(0) {
    texture = Object_manager::getTexture("ressources/soldier_1.png");
    status = WAIT;
}

void Soldier1::update(Game* game) {
    Enemy::update(game);
}

void Soldier1::draw(Game* game) {
    if (game->animation) {
        state++;
    }
    switch (status) {
    case WAIT:
        Entity::draw(game, Soldier1::walk_front_rects[0], 0.7);
        break;
    
    case WALK:
        state %= 4;
        Entity::draw(game, Soldier1::walk_front_rects[state], 0.7);
        break;
    
    case SHOOT:
        state %= 2;
        Entity::draw(game, Soldier1::shoot_rects[state], 0.7);
        break;
    
    default:
        break;
    }
    
};