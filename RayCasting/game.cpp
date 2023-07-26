#include <iostream>
#include <SFML/Graphics.hpp>

#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/game.hpp"



Game::Game() : map(this), player(this) {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray casting !", sf::Style::Close);
    window.setPosition(sf::Vector2i((desktop.width - WINDOW_WIDTH) / 2, (desktop.height - WINDOW_HEIGHT) / 2));
    window.setFramerateLimit(50);
    window.setKeyRepeatEnabled(false);
    window.setMouseCursorVisible(false);
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
    map.update();
    player.update();
}

void Game::render() {
    window.clear(sf::Color(50, 50, 50, 255));

    // display 3D
    player.draw();

    // display the map
    map.draw();

    window.display();
}
