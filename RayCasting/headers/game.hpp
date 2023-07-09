#pragma once

constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_WIDTH = 1280;


class Game {
public:
    Game();
    void run();

private:
    void handleEvents();
    void update();
    void render();

    sf::RenderWindow window;
    Player player;
};