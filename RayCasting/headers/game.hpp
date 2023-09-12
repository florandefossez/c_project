#pragma once

constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_WIDTH = 1080;
constexpr int MAP_HEIGHT = 64;
constexpr int MAP_WIDTH = 64;
constexpr int MINIMAP = 300;


class Game {
public:
    Game();
    ~Game();
    void run();

    Map map;
    Player player;
    Raycaster raycaster;
    Object_manager entities_manager;

    bool animation;
    SDL_Renderer* renderer;

    void handleEvents();
    void update();
    void render();

    float delta_time;

    std::array<uint32_t, WINDOW_WIDTH * WINDOW_HEIGHT> scene_pixels;

private:

    SDL_Texture* scene;
    SDL_Window* window;
    bool running;
    Uint32 prev_ticks;
};
