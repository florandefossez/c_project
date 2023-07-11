#pragma once

constexpr int WINDOW_HEIGHT = 800;
constexpr int WINDOW_WIDTH = 800;

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