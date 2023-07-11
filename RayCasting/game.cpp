#include <iostream>
#include <SFML/Graphics.hpp>


#include "headers/player.hpp"
#include "headers/game.hpp"


Game::Game() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray casting !", sf::Style::Close);
    window.setPosition(sf::Vector2i((desktop.width - WINDOW_WIDTH) / 2, (desktop.height - WINDOW_HEIGHT) / 2));
    window.setFramerateLimit(50);
    window.setKeyRepeatEnabled(false);
    window.setMouseCursorVisible(false);
    load_map();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
        }
    }
}

void Game::update() {
    player.update(&map);
}

void Game::render() {
    window.clear(sf::Color(50, 50, 50, 255));

    // display the map
    sf::RectangleShape wall_rect;
    wall_rect.setFillColor(sf::Color::Black);
    wall_rect.setSize(sf::Vector2f(WINDOW_WIDTH/32, WINDOW_HEIGHT/32));
    for (unsigned int i=0; i<32; i++) {
        for (unsigned int j=0; j<32; j++) {
            if (map[i][j] == wall) {
                wall_rect.setPosition(i*WINDOW_WIDTH/32, j*WINDOW_HEIGHT/32);
                window.draw(wall_rect);
            }
        }
    }

    player.draw(&window);
    window.display();
}


void Game::load_map() {
    std::cout << "load map\n";
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