#include <SFML/Graphics.hpp>
#include <cmath>

#include "headers/raycaster.hpp"
#include "headers/map.hpp"
#include "headers/player.hpp"
#include "headers/game.hpp"



Map::Map(Game* game) : game(game) {
    load_map();

    player_texture.loadFromFile("./ressources/player.png");
    player_sprite.setScale(0.5, 0.5);
    player_sprite.setOrigin(16,16);
    player_sprite.setTexture(player_texture);
    vision_field.resize(WINDOW_WIDTH + 1);
    vision_field.setPrimitiveType(sf::TriangleFan);
}

void Map::load_map() {
    sf::Image map_image;
    map_image.loadFromFile("./ressources/map.png");

    for (unsigned int i=0; i<32; i++) {
        for (unsigned int j=0; j<32; j++) {
            // if pixel is not transparent this is a wall
            if (map_image.getPixel(i,j).a == 255) {
                this->map[i][j] = wall;
            } else {
                this->map[i][j] = empty;
            }
        }
    }
}

bool Map::collide(float x, float y) {
    return (map[(unsigned int) floor(x)][(unsigned int) floor(y)] == wall);
}

bool Map::collide(unsigned int x, unsigned int y) {
    return (map[x][y] == wall);
}

void Map::update() {
    player_sprite.setPosition(game->player.position_x*MINIMAP/32.0, game->player.position_y*MINIMAP/32.0);
    player_sprite.setRotation(game->player.get_angle());
}

void Map::draw() {
    sf::RectangleShape wall_rect;
    wall_rect.setFillColor(sf::Color::Black);
    wall_rect.setSize(sf::Vector2f((float) MINIMAP/ 32.0, (float) MINIMAP/32.0));
    for (unsigned int i=0; i<32; i++) {
        for (unsigned int j=0; j<32; j++) {
            if (collide(i,j)) {
                wall_rect.setPosition((float) MINIMAP/32.0 * i, (float) MINIMAP/32.0 * j);
                game->window.draw(wall_rect);
            }
        }
    }

    vision_field[0].position = player_sprite.getPosition();
    game->window.draw(vision_field);
    game->window.draw(player_sprite);
}