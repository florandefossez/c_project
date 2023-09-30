#pragma once
#include <vector>
#include <map>
#include <string>
#include <array>

class Player;
class Game;

class Hud {
public:

    Hud(Game* game);
    void load(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer);
    void draw_menu(SDL_Renderer* renderer);
    void draw_option(SDL_Renderer* renderer);
    void update(bool tick, Player* player);
    void handleEvents_menu(SDL_Event* event);
    void handleEvents_option(SDL_Event* event);
    void draw_text(SDL_Renderer* renderer, std::string text, int x, int y, int h);


private:
    Game* game;
    SDL_Texture* texture;
    SDL_Texture* menu_background;
    static std::array<SDL_Rect, 8> faces1_rects;
    static std::array<SDL_Rect, 8> faces2_rects;
    static std::array<SDL_Rect, 8> faces3_rects;
    static std::array<SDL_Rect, 8> faces4_rects;
    static std::array<SDL_Rect, 8> faces5_rects;
    static std::array<SDL_Rect, 10> big_numbers;
    static std::array<SDL_Rect, 10> grey_little_numbers;
    static std::array<SDL_Rect, 10> yellow_little_numbers;
    static std::array<SDL_Rect, 96> letters;
    
    int menu_index;
    int face;
    int option_index;
};