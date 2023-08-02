#include <SFML/Graphics.hpp>

#include "headers/raycaster.hpp"
#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/entities_manager.hpp"
#include "headers/game.hpp"



Raycaster::Raycaster(Game* game) : game(game) {

    brick_texture.loadFromFile("./ressources/redbrick.png");
    stone.loadFromFile("./ressources/greystone.png");
    mosse.loadFromFile("./ressources/mossy.png");
    floor_image.create(WINDOW_WIDTH, WINDOW_HEIGHT/2-1);
    floor_texture.create(WINDOW_WIDTH, WINDOW_HEIGHT/2-1);
}


void Raycaster::update() {
    raycast_floor();
    raycast_wall();
}

void Raycaster::draw() {
    draw_floor();
    draw_wall();
}


void Raycaster::raycast_floor() {

    for (int i=0; i<WINDOW_WIDTH; i++) {
        for (int j=0; j<WINDOW_HEIGHT/2-1; j++) {
            floor_image.setPixel(i,j,sf::Color::Red);
        }
    }

    // again we use rays to find the coordinate of the floor
    float left_ray_x = game->player.dir_x - game->player.plane_x;
    float left_ray_y = game->player.dir_y - game->player.plane_y;
    float right_ray_x = game->player.dir_x + game->player.plane_x;
    float right_ray_y = game->player.dir_y + game->player.plane_y;

    // p index of the scanline from the center of the screen
    for (int p=1; p<WINDOW_HEIGHT/2; p++) {

        // horizontal distance from the player to the floor line (x distance of the ray)
        float floor_distance = game->player.position_z*WINDOW_HEIGHT/p;

        // floor coordinate of the left ray
        float floor_x = game->player.position_x + floor_distance * left_ray_x;
        float floor_y = game->player.position_y + floor_distance * left_ray_y;

        // floor step to go to the next right pixel on the screen
        float floor_step_x = floor_distance * (right_ray_x - left_ray_x) / WINDOW_WIDTH;
        float floor_step_y = floor_distance * (right_ray_y - left_ray_y) / WINDOW_WIDTH;

        for (int x=0; x < WINDOW_WIDTH; x++) {

            // the cell coord is simply got from the integer parts of floorX and floorY
            int current_cell_x = (int)(floor_x);
            int current_cell_y = (int)(floor_y);

            // get the texture coordinate from the fractional part
            unsigned int tx = (unsigned int) std::abs(64.0 * (floor_x - current_cell_x));
            unsigned int ty = (unsigned int) std::abs(64.0 * (floor_y - current_cell_y));

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            // choose texture and draw the pixel on the image
            if ((current_cell_x + current_cell_y)%2 == 1) {
                floor_image.setPixel(x, p-1, mosse.getPixel(tx,ty));
                // floor_image.setPixel(x, p-1, sf::Color::Black);
            } else {
                floor_image.setPixel(x, p-1, stone.getPixel(tx,ty));
                // floor_image.setPixel(x, p-1, sf::Color::White);
            }
        }
    }
}


void Raycaster::raycast_wall() {

    char collision_side;
    
    for (int r=0; r<WINDOW_WIDTH; r++) {

        // direction step
        int step_x;
        int step_y;

        // euclidian ray length between the position and the wall
        float ray_length = 0;

        // x and y ray sides
        float x_ray_length = 0;
        float y_ray_length = 0;

        // x coord of the ray in the camera plane in [-1,1]
        float camera = 2 * (float) r / (float) WINDOW_WIDTH - 1;

        // ray directions
        float ray_dir_x = game->player.dir_x + game->player.plane_x * camera;
        float ray_dir_y = game->player.dir_y + game->player.plane_y * camera;

        // ray lengths per x,y step
        float x_ray_unit_length = std::abs(1 / ray_dir_x);
        float y_ray_unit_length = std::abs(1 / ray_dir_y);

        // player cell location
        unsigned int current_cell_x = (unsigned int) game->player.position_x;
        unsigned int current_cell_y = (unsigned int) game->player.position_y;

        if (ray_dir_x > 0) { // we look at west
            x_ray_length += x_ray_unit_length * (1 - (game->player.position_x - (float) current_cell_x));
            step_x = 1;

        } else if (ray_dir_x < 0) { //we look at east
            x_ray_length += x_ray_unit_length * (game->player.position_x - (float) current_cell_x);
            step_x = -1;
        
        } else {
            step_x = 0;
        }

        if (ray_dir_y > 0) { // we look at south
            y_ray_length += y_ray_unit_length * (1 - (game->player.position_y - (float) current_cell_y));
            step_y = 1;
            
        } else if (ray_dir_y < 0) { //we look at north
            y_ray_length += y_ray_unit_length * (game->player.position_y - (float) current_cell_y);
            step_y = -1;

        } else {
            step_y = 0;
        }


        // we MUST break this loop
        for (int i=0; i<50; i++) {
            if (x_ray_length < y_ray_length) {
                ray_length = x_ray_length;
                x_ray_length += x_ray_unit_length;

                current_cell_x += step_x;
                collision_side = 'x';
            }
            else {
                ray_length = y_ray_length;
                y_ray_length += y_ray_unit_length;

                current_cell_y += step_y;
                collision_side = 'y';
            }
            if (game->map.collide(current_cell_x,current_cell_y)){
                break;
            }
        }

        game->map.vision_field[r+1].position = sf::Vector2f(
            (game->player.position_x + ray_length * ray_dir_x) * MINIMAP / 32.0,
            (game->player.position_y + ray_length * ray_dir_y) * MINIMAP / 32.0
        );

        rays_lenght[r] = ray_length;


        // we don't use the real ray length to avoid fisheye effect
        if (collision_side == 'x') {
            perp_rays_lenght[r] = x_ray_length - x_ray_unit_length;
            texture_offset[r] = game->player.position_y + ray_length * ray_dir_y - (float) current_cell_y;
        } else {
            perp_rays_lenght[r] = y_ray_length - y_ray_unit_length;
            texture_offset[r] = game->player.position_x + ray_length * ray_dir_x - (float) current_cell_x;
        }
    }
}



void Raycaster::draw_floor() {
    sf::Sprite floor_sprite;
    floor_texture.update(floor_image);
    floor_sprite.setTexture(floor_texture);
    floor_sprite.setPosition(0, WINDOW_HEIGHT/2+1);

    game->window.draw(floor_sprite);
}

void Raycaster::draw_wall() {
    sf::Sprite wall;
    wall.setTexture(brick_texture);
    for (int r=0; r<WINDOW_WIDTH; r++) {
        wall.setTextureRect(sf::IntRect(texture_offset[r]*64.0, 0, 1, 64));
        wall.setScale(sf::Vector2f(1, (float) WINDOW_HEIGHT/perp_rays_lenght[r]/64.0));
        wall.setPosition(sf::Vector2f(r, (WINDOW_HEIGHT - WINDOW_HEIGHT/perp_rays_lenght[r])/2));

        game->window.draw(wall);
    }
}