#pragma once
#include <vector>
#include <map>
#include <string>
#include <array>

class Player;

class Hud {
public:

    Hud(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer);
    void update(bool tick, Player* player);

private:
    SDL_Texture* texture;
    static std::array<SDL_Rect, 8> faces1_rects;
    static std::array<SDL_Rect, 8> faces2_rects;
    static std::array<SDL_Rect, 8> faces3_rects;
    static std::array<SDL_Rect, 8> faces4_rects;
    static std::array<SDL_Rect, 8> faces5_rects;
    static std::array<SDL_Rect, 10> big_numbers;
    unsigned int face;

    int health;
};