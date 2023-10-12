#pragma once

#include <SDL2/SDL_mixer.h>


constexpr int WINDOW_HEIGHT = 720;
constexpr int WINDOW_WIDTH = 1080;

typedef enum game_state_ {
    PLAY,
    LEVEL_MENU,
    MENU,
    OPTIONS
} game_state_t;


class Game {
public:
    Game();
    ~Game();
    void run();

    int width;
    game_state_t state;

    Map map;
    Player player;
    Raycaster raycaster;
    Object_manager entities_manager;
    Hud hud;

    bool animation;
    SDL_Renderer* renderer;

    void handleEvents();
    void update();
    void render();

    float delta_time;
    void toggleFullscreen();
    void update_width(int w);
    void start_level(int level_id);
    void stop_run();
    void menu();

    uint32_t* scene_pixels;

    // keys
    SDL_Keycode move_forward;
    SDL_Keycode move_backward;
    SDL_Keycode move_left;
    SDL_Keycode move_right;
    SDL_Keycode shoot;
    SDL_Keycode interact;
    SDL_Keycode switch_weapon;

private:

    SDL_Texture* scene;
    SDL_Window* window;
    bool running;
    Uint32 prev_ticks;

    Mix_Music* menu_music;
    Mix_Music* game_music;
};
