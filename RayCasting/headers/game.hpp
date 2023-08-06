#pragma once

constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_WIDTH = 1080;
constexpr int MAP_HEIGHT = 32;
constexpr int MAP_WIDTH = 32;
constexpr int MINIMAP = 300;

#include <SDL2/SDL.h>

class Game {
public:
    Game();
    ~Game();
    void run();

    Map map;
    Player player;
    Raycaster raycaster;
    // Object_manager entities_manager;

    bool animation;

private:
    void handleEvents();
    void update();
    void render();

    SDL_Window* window;
    SDL_Renderer* renderer;
    float delta_time;
    bool running;
    Uint32 prev_ticks;
};
