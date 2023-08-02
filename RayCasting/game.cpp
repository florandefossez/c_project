#include <iostream>
#include <SFML/Graphics.hpp>

#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/raycaster.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"



Game::Game() : map(this), player(this), raycaster(this), entities_manager(this) {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray casting !", sf::Style::Close);
    window.setPosition(sf::Vector2i((desktop.width - WINDOW_WIDTH) / 2, (desktop.height - WINDOW_HEIGHT) / 2));
    window.setKeyRepeatEnabled(false);
    window.setMouseCursorVisible(false);
    delta_time = 1;
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
    raycaster.update();
    entities_manager.update();

    delta_time = clock.restart().asSeconds();

    // display FPS
    static int frame = 0;
    static float SPF = 0;

    SPF += delta_time;
    if (frame%100 == 0) {
        window.setTitle(std::to_string((int) (100.0/SPF)));
        frame = 0;
        SPF = 0;
        clock.restart();
    }
    frame++;

}

void Game::render() {
    window.clear(sf::Color(50, 50, 50, 255));

    // display 3D
    raycaster.draw();
    entities_manager.draw();

    // display weapon
    player.draw();

    // display the map
    map.draw();

    window.display();
}
