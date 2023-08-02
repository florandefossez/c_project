#pragma once

class Game;

class Raycaster {
public:
    Raycaster(Game* game);
    void update();
    void draw();

    float rays_lenght[1080];

private:
    void raycast_wall();
    void raycast_floor();

    void draw_wall();
    void draw_floor();

    Game* game;

    // floor
    sf::Image floor_image;
    sf::Texture floor_texture;

    // wall

    sf::Texture brick_texture;
    sf::Image stone;
    sf::Image mosse;

    float perp_rays_lenght[1080];
    float texture_offset[1080];

};