#pragma once
#include <vector>
#include <map>
#include <string>
#include <array>


class Hud {
public:

    Hud(SDL_Renderer* renderer);
    void draw(SDL_Renderer* renderer);
    void update(bool tick);

private:
    SDL_Texture* texture;
    static std::array<SDL_Rect, 8> faces1_rects;
    unsigned int face;
};