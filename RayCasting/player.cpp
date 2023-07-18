#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "headers/player.hpp"
#include "headers/game.hpp"


Player::Player() {

    player_texture.loadFromFile("./ressources/player.png");
    player_sprite.setScale(0.5, 0.5);
    player_sprite.setOrigin(16,16);
    player_sprite.setTexture(player_texture);
    vision_field.resize(WINDOW_WIDTH + 1);
    vision_field.setPrimitiveType(sf::TriangleFan);

    position_x = 2.0;
    position_y = 3.0;
    player_sprite.setPosition(position_x*MINIMAP/32.0, position_y*MINIMAP/32.0);
}

inline bool collide(std::array<std::array<cell_t, 32>, 32>* map, float x, float y) {
    return ((*map)[(unsigned int) floor(x)][(unsigned int) floor(y)] == wall);
}


void Player::update(std::array<std::array<cell_t, 32>, 32>* map) {


    float move_x = 0;
    float move_y = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        move_x += PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
        move_y += PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        move_x += -PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
        move_y += -PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        move_x += PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
        move_y += -PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move_x += -PLAYER_VELOCITY*sin(player_sprite.getRotation()*3.1415/180);
        move_y += PLAYER_VELOCITY*cos(player_sprite.getRotation()*3.1415/180);
    }

    player_sprite.setColor(sf::Color::Green);
    if (collide(map, move_x+position_x, position_y)) {
        player_sprite.setColor(sf::Color::Red);
        if (move_x>0) {
            move_x = floor((position_x+move_x)) - position_x - 0.01;
        } else if (move_x<0) {
            move_x = ceil((position_x+move_x)) - position_x + 0.01;
        }
    }
    if (collide(map, position_x, position_y+move_y)) {
        player_sprite.setColor(sf::Color::Red);
        if (move_y>0) {
            move_y = floor((position_y+move_y)) - position_y - 0.01;
        } else if (move_y<0) {
            move_y = ceil((position_y+move_y)) - position_y + 0.01;
        }
    }
    position_x += move_x;
    position_y += move_y;

    player_sprite.setPosition((position_x) * MINIMAP/32.0, (position_y) * MINIMAP/32.0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
        player_sprite.setRotation(player_sprite.getRotation() + PLAYER_ROTATION_VELOCITY);
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        player_sprite.setRotation(player_sprite.getRotation() - PLAYER_ROTATION_VELOCITY);
        
    ray_casting(map);
}

void Player::draw(sf::RenderWindow* window) {
    // player_sprite.setRotation(player_sprite.getRotation() + (sf::Mouse::getPosition(*window).x-WINDOW_WIDTH/2)/10);
    // sf::Mouse::setPosition(sf::Vector2i(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), *window);
    window->draw(vision_field);
    window->draw(player_sprite);
}

void Player::draw3D(sf::RenderWindow* window) {
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Green);
    for (int r=0; r<WINDOW_WIDTH; r++) {
        rect.setSize(sf::Vector2f(1, 1.5*WINDOW_HEIGHT/rays_lenght[r]));
        rect.setPosition(sf::Vector2f(r, (WINDOW_HEIGHT - 1.5*WINDOW_HEIGHT/rays_lenght[r])/2));
        if (collision_side[r] == 'x')
            rect.setFillColor(sf::Color(0,255*(1-rays_lenght[r]/32),0));
        else
            rect.setFillColor(sf::Color(0,0.5 * 255*(1-rays_lenght[r]/32),0));

        window->draw(rect);
    }
}


// DDA algorithm
void Player::ray_casting(std::array<std::array<cell_t, 32>, 32>* map) {
    int step_x = 0;
    int step_y = 0;

    float FOV = 70;

    vision_field[0].position = player_sprite.getPosition();

    for (int r=0; r<WINDOW_WIDTH; r++) {

        float ray_length = 0;
        float x_ray_length = 0;
        float y_ray_length = 0;

        float angle = (player_sprite.getRotation() + ((float) r - (float) WINDOW_WIDTH/2.0)*FOV / WINDOW_WIDTH )/180.0*3.1416;

        float cos_ = cos(angle);
        float sin_ = sin(angle);

        float x_ray_unit_length = 1 / cos_;
        float y_ray_unit_length = 1 / sin_;
        x_ray_unit_length = x_ray_unit_length>0 ? x_ray_unit_length : -x_ray_unit_length;
        y_ray_unit_length = y_ray_unit_length>0 ? y_ray_unit_length : -y_ray_unit_length;

        float current_cell_x = floor(position_x);
        float current_cell_y = floor(position_y);

        if (cos_ > 0) { // we look at west
            x_ray_length += x_ray_unit_length * (1 - (position_x - current_cell_x));
            step_x = 1;

        } else if (cos_ < 0) { //we look at east
            x_ray_length += x_ray_unit_length * (position_x - current_cell_x);
            step_x = -1;
        
        } else {
            step_x = 0;
        }

        if (sin_ > 0) { // we look at south
            y_ray_length += y_ray_unit_length * (1 - (position_y - current_cell_y));
            step_y = 1;
            
        } else if (sin_ < 0) { //we look at north
            y_ray_length += y_ray_unit_length * (position_y - current_cell_y);
            step_y = -1;

        } else {
            step_y = 0;
        }


        // we MUST break this loop
        for (int i=0; i<50; i++) {
            if (x_ray_length < y_ray_length) {
                ray_length = x_ray_length;
                x_ray_length += x_ray_unit_length;

                current_cell_x += step_x;
                collision_side[r] = 'x';
            }
            else {
                ray_length = y_ray_length;
                y_ray_length += y_ray_unit_length;

                current_cell_y += step_y;
                collision_side[r] = 'y';
            }
            if (wall == (*map)[(unsigned int) current_cell_x][(unsigned int) current_cell_y]){
                break;
            }
        }

        vision_field[r+1].position = sf::Vector2f((position_x+ray_length*cos_) * MINIMAP/32.0, (position_y+ray_length*sin_) * MINIMAP/32.0);

        rays_lenght[r] = ray_length*cosf(((float) r - (float) WINDOW_WIDTH/2.0)*FOV / (float) WINDOW_WIDTH/180*3.1416);
    }

}

