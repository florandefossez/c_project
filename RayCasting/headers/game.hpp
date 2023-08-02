#pragma once

constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_WIDTH = 1080;
constexpr int MAP_HEIGHT = 32;
constexpr int MAP_WIDTH = 32;
constexpr int MINIMAP = 300;


class Game {
public:
    Game();
    void run();

    Map map; 
    Player player;
    Raycaster raycaster;
    Object_manager entities_manager;
    
    sf::RenderWindow window;
    float delta_time;

private:
    void handleEvents();
    void update();
    void render();
    void load_map();

    sf::Clock clock;
};