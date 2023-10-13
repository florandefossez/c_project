#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include "headers/raycaster.hpp"
#include "headers/player.hpp"
#include "headers/map.hpp"
#include "headers/entities_manager.hpp"
#include "headers/hud.hpp"
#include "headers/game.hpp"



Raycaster::Raycaster(Game* game) : game(game) {}


void Raycaster::load() {

    char file_name[30];
    SDL_Surface* tmp;

    for (int i=0; i<10; i++) {
        sprintf(file_name, "ressources/textures/%d.png", i);
        tmp = IMG_Load(file_name);
        textures[i] = (Uint32*) SDL_ConvertSurfaceFormat(tmp, SDL_PIXELFORMAT_RGBA8888, 0)->pixels;
        SDL_FreeSurface(tmp);
    }

    rays_lenght = new float[game->width];

}

void Raycaster::start() {
    opening_state = 0;
    opening_door_x = 0;
    opening_door_y = 0;

    // ABGR
    fog_color[0] = 255;
    fog_color[1] = 0x06;
    fog_color[2] = 0x2c;
    fog_color[3] = 0x3e;
    
}

void Raycaster::update_width() {
    free(rays_lenght);
    rays_lenght = new float[game->width];
}


void Raycaster::update() {
    if (opening_state > 0 && game->animation) {
        opening_state -= 0.1;
    }
    if (opening_state <= 0) {
        game->map.map[opening_door_x][opening_door_y].is_wall = false;
        game->map.map[opening_door_x][opening_door_y].is_door = false;
        opening_state = 0;
    }
}

void Raycaster::draw() {
    draw_floor();
    draw_wall();
}


void Raycaster::draw_floor() {

    // again we use rays to find the coordinate of the floor
    float left_ray_x = game->player.dir_x - game->player.plane_x;
    float left_ray_y = game->player.dir_y - game->player.plane_y;
    float right_ray_x = game->player.dir_x + game->player.plane_x;
    float right_ray_y = game->player.dir_y + game->player.plane_y;

    int height = 2 * game->width / 3;

    // p index of the scanline from the center of the screen
    for (int p=1; p<height/2; p++) {

        // horizontal distance from the player to the floor line (x distance of the ray)
        float floor_distance = 15.f / 14.f * height / p * game->player.position_z;

        // floor coordinate of the left ray
        float floor_x = game->player.position_x + floor_distance * left_ray_x;
        float floor_y = game->player.position_y + floor_distance * left_ray_y;

        // floor step to go to the next right pixel on the screen
        float floor_step_x = floor_distance * (right_ray_x - left_ray_x) / game->width;
        float floor_step_y = floor_distance * (right_ray_y - left_ray_y) / game->width;

        int d = (int) floor_distance;
        if (d>32) d=32;
        int weighted_fog[4] = {d * fog_color[0], d * fog_color[1], d * fog_color[2], d * fog_color[3]};
        d = 32 - d;

        for (int x=0; x < game->width; x++) {

            // the cell coord is simply got from the integer parts of floorX and floorY
            int current_cell_x = (int)(floor_x);
            int current_cell_y = (int)(floor_y);

            if (
                current_cell_x<0 || current_cell_x>=64 ||
                current_cell_y<0 || current_cell_y>=64 ||
                (
                    game->map.map[current_cell_x][current_cell_y].is_wall &&
                    !game->map.map[current_cell_x][current_cell_y].is_door
                )) {
                floor_x += floor_step_x;
                floor_y += floor_step_y;
                continue;
            }

            // get the texture coordinate from the fractional part
            int tx = (int) std::abs(64.f * (floor_x - current_cell_x));
            int ty = (int) std::abs(64.f * (floor_y - current_cell_y));

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            // choose texture and draw the pixel on the image
            Uint8 *color = (Uint8 *) &textures[
                game->map.map[current_cell_x][current_cell_y].texture_id
            ][tx + 64*ty];
            Uint8 *target_color = (Uint8 *) &game->scene_pixels[x + (p-1) * game->width + height/2 * game->width];
            
            target_color[0] = 255;
            target_color[1] = (color[1] * d + weighted_fog[1]) >> 5;
            target_color[2] = (color[2] * d + weighted_fog[2]) >> 5;
            target_color[3] = (color[3] * d + weighted_fog[3]) >> 5;
        }
    }
}


void Raycaster::draw_wall() {

    int height = 2 * game->width / 3;
    char collision_side;
    float perp_rays_lenght;
    float texture_offset;

    for (int r=0; r<game->width; r++) {

        int texture_id;

        // direction step
        int step_x;
        int step_y;

        // euclidian ray length between the position and the wall
        float ray_length = 0;

        // x and y ray sides
        float x_ray_length = 0;
        float y_ray_length = 0;

        // x coord of the ray in the camera plane in [-1,1]
        float camera = 2.f * (float) r / (float) game->width - 1.f;

        // ray directions
        float ray_dir_x = game->player.dir_x + game->player.plane_x * camera;
        float ray_dir_y = game->player.dir_y + game->player.plane_y * camera;

        // ray lengths per x,y step
        float x_ray_unit_length = std::abs(1 / ray_dir_x);
        float y_ray_unit_length = std::abs(1 / ray_dir_y);

        // player cell location
        int current_cell_x = (int) game->player.position_x;
        int current_cell_y = (int) game->player.position_y;

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
raycast_label:
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
        
        // targeted wall
        if (r == game->width/2) {
            targeted_wall_x = current_cell_x;
            targeted_wall_y = current_cell_y;
        }

        texture_id = game->map.map[current_cell_x][current_cell_y].texture_id;

        // we don't use the real ray length to avoid fisheye effect
        if (collision_side == 'x') {
            perp_rays_lenght = x_ray_length - x_ray_unit_length;
            texture_offset = game->player.position_y + ray_length * ray_dir_y - (float) current_cell_y;

            if (game->map.map[current_cell_x][current_cell_y].is_door) {
                float cell_y = game->player.position_y + ray_length * ray_dir_y + std::abs(0.4f/ray_dir_x) * ray_dir_y;
                if (int(cell_y) == current_cell_y) {
                    texture_offset = cell_y - floor(cell_y);
                    if (opening_state > 0 && current_cell_x == opening_door_x && current_cell_y == opening_door_y && texture_offset > opening_state) {
                        goto raycast_label;
                    }
                    perp_rays_lenght += std::abs(0.4f/ray_dir_x);
                    texture_id = 9;
                } else {
                    texture_id = game->map.map[current_cell_x][current_cell_y-1].texture_id;
                    ray_length = y_ray_length;
                    y_ray_length += y_ray_unit_length;

                    current_cell_y += step_y;
                    collision_side = 'y';
                    perp_rays_lenght = y_ray_length - y_ray_unit_length;
                    texture_offset = game->player.position_x + ray_length * ray_dir_x - (float) current_cell_x;
                }
            }
            
        } else {
            perp_rays_lenght = y_ray_length - y_ray_unit_length;
            texture_offset = game->player.position_x + ray_length * ray_dir_x - (float) current_cell_x;

            if (game->map.map[current_cell_x][current_cell_y].is_door) {
                float cell_x = game->player.position_x + ray_length * ray_dir_x + std::abs(0.4f/ray_dir_y) * ray_dir_x;
                if (int(cell_x) == current_cell_x) {
                    texture_offset = cell_x - floor(cell_x);
                    if (opening_state > 0 && current_cell_x == opening_door_x && current_cell_y == opening_door_y && texture_offset > opening_state) {
                        goto raycast_label;
                    }
                    perp_rays_lenght += std::abs(0.4f/ray_dir_y);
                    texture_id = 9;
                } else {
                    texture_id = game->map.map[current_cell_x-1][current_cell_y].texture_id;
                    ray_length = x_ray_length;
                    x_ray_length += x_ray_unit_length;

                    current_cell_x += step_x;
                    collision_side = 'x';
                    perp_rays_lenght = x_ray_length - x_ray_unit_length;
                    texture_offset = game->player.position_y + ray_length * ray_dir_y - (float) current_cell_y;
                }
            }

        }

        rays_lenght[r] = ray_length;
        float wall_height = 15.f / 14.f * height / perp_rays_lenght;

        // where to start and end the dawing of the strip
        int start,end;
        if (height<wall_height) {
            start = 0;
            end = height;
        } else {
            start = (height - wall_height) / 2;
            end = start + wall_height;
        }

        // How much to increase the texture coordinate per screen pixel
        float step = 64.f / wall_height;
        float texPos = (start + (wall_height - height) / 2) * step;

        // fog
        int d = (int) ray_length;
        if (d>32) d=32;
        int weighted_fog[4] = {d * fog_color[0], d * fog_color[1], d * fog_color[2], d * fog_color[3]};
        d = 32 - d;

        // draw
        for (int y=start; y<end; y++) {
            int texY = (int)texPos & (64 - 1);
            texPos += step;

            Uint8 *color = (Uint8 *) &textures[texture_id][
                static_cast<unsigned int>(64.f * texY + texture_offset*64.f)
            ];
            Uint8 *target_color = (Uint8 *) &game->scene_pixels[r + y * game->width];
            
            target_color[0] = 255;
            target_color[1] = (color[1] * d + weighted_fog[1]) >> 5;
            target_color[2] = (color[2] * d + weighted_fog[2]) >> 5;
            target_color[3] = (color[3] * d + weighted_fog[3]) >> 5;
        }
    }
}

void Raycaster::trigger() {
    if (!game->map.map[targeted_wall_x][targeted_wall_y].is_door || rays_lenght[game->width/2]>1.5f || opening_state!=0) return;
    opening_door_x = targeted_wall_x;
    opening_door_y = targeted_wall_y;
    opening_state = 1.f;
}