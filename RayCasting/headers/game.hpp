#pragma once

constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_WIDTH = 1080;
constexpr int MAP_HEIGHT = 64;
constexpr int MAP_WIDTH = 64;


class Game {
public:
    Game();
    ~Game();
    void run();

    int width;

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
    void toggleFullscreen();
    void update_width(int w);

    uint32_t* scene_pixels;

private:

    SDL_Texture* scene;
    SDL_Window* window;
    bool running;
    Uint32 prev_ticks;
};
