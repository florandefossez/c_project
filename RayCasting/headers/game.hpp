#pragma once

#include <array>

constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_WIDTH = 1080;
constexpr int MAP_HEIGHT = 32;
constexpr int MAP_WIDTH = 32;
constexpr int MINIMAP = 300;

typedef enum cell_ : short {
    empty,
    wall,
} cell_t;


class Game {
public:
    Game();
    void run();

private:
    void handleEvents();
    void update();
    void render();
    void load_map();

    sf::RenderWindow window;
    Player player;
    std::array<std::array<cell_t,32>,32> map;
};