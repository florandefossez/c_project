#include <SFML/Graphics.hpp>
#include <iostream>
#include "headers/player.hpp"
#include "headers/game.hpp"

float FOV = 70.0;



Player::Player() {

    player_texture.loadFromFile("./ressources/player.png");
    player_sprite.setScale(0.5, 0.5);
    player_sprite.setOrigin(16,16);
    player_sprite.setTexture(player_texture);
    vision_field.resize(WINDOW_WIDTH + 1);
    vision_field.setPrimitiveType(sf::TriangleFan);

    position_x = 2.0;
    position_y = 3.0;
    position_z = 0.5;
    player_sprite.setPosition(position_x*MINIMAP/32.0, position_y*MINIMAP/32.0);

    dir_x = 1.0;
    dir_y = 0.0;
    dir_z = 0.0;
    player_sprite.setRotation(atanf(dir_y/dir_x)*3.1415/180.0);

    plane_x = 0.0;
    plane_y = 1.0;
    plane_z = 0.0;
}

inline bool collide(std::array<std::array<cell_t, 32>, 32>* map, float x, float y) {
    return ((*map)[(unsigned int) floor(x)][(unsigned int) floor(y)] == wall);
}


void Player::update(std::array<std::array<cell_t, 32>, 32>* map) {


    float move_x = 0;
    float move_y = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        move_x += PLAYER_VELOCITY*dir_x;
        move_y += PLAYER_VELOCITY*dir_y;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        move_x += -PLAYER_VELOCITY*dir_x;
        move_y += -PLAYER_VELOCITY*dir_y;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        move_x += PLAYER_VELOCITY*dir_y;
        move_y += -PLAYER_VELOCITY*dir_x;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        move_x += -PLAYER_VELOCITY*dir_y;
        move_y += PLAYER_VELOCITY*dir_x;
    }

    player_sprite.setColor(sf::Color::Green);
    if (collide(map, move_x+position_x, position_y)) {
        player_sprite.setColor(sf::Color::Red);
        if (move_x>0) {
            move_x = floor((position_x+move_x)) - position_x - 0.01;
        } else if (move_x<0) {
            move_x = ceil((position_x+move_x)) - position_x + 0.01;
        }
    }
    if (collide(map, position_x, position_y+move_y)) {
        player_sprite.setColor(sf::Color::Red);
        if (move_y>0) {
            move_y = floor((position_y+move_y)) - position_y - 0.01;
        } else if (move_y<0) {
            move_y = ceil((position_y+move_y)) - position_y + 0.01;
        }
    }
    position_x += move_x;
    position_y += move_y;

    player_sprite.setPosition((position_x) * MINIMAP/32.0, (position_y) * MINIMAP/32.0);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        float tmp_dir = COS_PLAYER_ROTATION*dir_x - SIN_PLAYER_ROTATION*dir_y;
        dir_y = SIN_PLAYER_ROTATION*dir_x + COS_PLAYER_ROTATION*dir_y;
        dir_x = tmp_dir;

        tmp_dir = COS_PLAYER_ROTATION*plane_x - SIN_PLAYER_ROTATION*plane_y;
        plane_y = SIN_PLAYER_ROTATION*plane_x + COS_PLAYER_ROTATION*plane_y;
        plane_x = tmp_dir;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        float tmp_dir = COS_PLAYER_ROTATION*dir_x + SIN_PLAYER_ROTATION*dir_y;
        dir_y = - SIN_PLAYER_ROTATION*dir_x + COS_PLAYER_ROTATION*dir_y;
        dir_x = tmp_dir;

        tmp_dir = COS_PLAYER_ROTATION*plane_x + SIN_PLAYER_ROTATION*plane_y;
        plane_y = - SIN_PLAYER_ROTATION*plane_x + COS_PLAYER_ROTATION*plane_y;
        plane_x = tmp_dir;
    }

    float angle = atanf(dir_y/dir_x)*180.0/3.14159;
    if (dir_x < 0) angle -= 180;
    if (angle < 0) angle += 360;
    
    player_sprite.setRotation(angle);
        
    ray_casting(map);
}

void Player::draw(sf::RenderWindow* window) {
    // player_sprite.setRotation(player_sprite.getRotation() + (sf::Mouse::getPosition(*window).x-WINDOW_WIDTH/2)/10);
    // sf::Mouse::setPosition(sf::Vector2i(WINDOW_WIDTH/2, WINDOW_HEIGHT/2), *window);
    vision_field[0].position = player_sprite.getPosition();
    window->draw(vision_field);
    window->draw(player_sprite);
}

void Player::draw3D(sf::RenderWindow* window) {
    // draw floor
    sf::Image floor_image;
    floor_image.create(WINDOW_WIDTH, WINDOW_HEIGHT/2-1);

    for (int i=0; i<WINDOW_WIDTH; i++) {
        for (int j=0; j<WINDOW_HEIGHT/2-1; j++) {
            floor_image.setPixel(i,j,sf::Color::Red);
        }
    }

    // again we use rays to find the coordinate of the floor
    float left_ray_x = dir_x - plane_x;
    float left_ray_y = dir_y - plane_y;
    float right_ray_x = dir_x + plane_x;
    float right_ray_y = dir_y + plane_y;

    // p index of the scanline from the center of the screen
    for (int p=1; p<WINDOW_HEIGHT/2; p++) {

        // horizontal distance from the player to the floor line (x distance of the ray)
        float floor_distance = position_z*WINDOW_HEIGHT/p;

        // floor coordinate of the left ray
        float floor_x = position_x + floor_distance * left_ray_x;
        float floor_y = position_y + floor_distance * left_ray_y;

        // floor step to go to the next right pixel on the screen
        float floor_step_x = floor_distance * (right_ray_x - left_ray_x) / WINDOW_WIDTH;
        float floor_step_y = floor_distance * (right_ray_y - left_ray_y) / WINDOW_WIDTH;

        for (int x=0; x < WINDOW_WIDTH; x++) {

            // the cell coord is simply got from the integer parts of floorX and floorY
            int current_cell_x = (int)(floor_x);
            int current_cell_y = (int)(floor_y);

            // get the texture coordinate from the fractional part
            // int tx = (int)(texWidth * (floorX - current_cell_x)) & (texWidth - 1);
            // int ty = (int)(texHeight * (floorY - current_cell_y)) & (texHeight - 1);

            floor_x += floor_step_x;
            floor_y += floor_step_y;

            // choose texture and draw the pixel
            if ((current_cell_x + current_cell_y)%2 == 1) {
                floor_image.setPixel(x, p-1, sf::Color::Red);
            } else {
                floor_image.setPixel(x, p-1, sf::Color::Black);
            }
        }
    }

    sf::Texture t;
    t.create(WINDOW_WIDTH, WINDOW_HEIGHT/2-1);
    t.update(floor_image);
    sf::Sprite floor_sprite;
    floor_sprite.setTexture(t);
    floor_sprite.setPosition(0, WINDOW_HEIGHT/2+1);

    window->draw(floor_sprite);


    // draw walls
    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Green);
    for (int r=0; r<WINDOW_WIDTH; r++) {
        rect.setSize(sf::Vector2f(1, WINDOW_HEIGHT/rays_lenght[r]));
        rect.setPosition(sf::Vector2f(r, (WINDOW_HEIGHT - WINDOW_HEIGHT/rays_lenght[r])/2));
        if (collision_side[r] == 'x')
            rect.setFillColor(sf::Color(0,255*(1-rays_lenght[r]/32),0));
        else
            rect.setFillColor(sf::Color(0,0.5 * 255*(1-rays_lenght[r]/32),0));

        window->draw(rect);
    }
}


// DDA algorithm
void Player::ray_casting(std::array<std::array<cell_t, 32>, 32>* map) {
    
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
        float ray_dir_x = dir_x + plane_x * camera;
        float ray_dir_y = dir_y + plane_y * camera;

        // ray lengths per x,y step
        float x_ray_unit_length = std::abs(1 / ray_dir_x);
        float y_ray_unit_length = std::abs(1 / ray_dir_y);

        // player cell location
        int current_cell_x = int(position_x);
        int current_cell_y = int(position_y);

        if (ray_dir_x > 0) { // we look at west
            x_ray_length += x_ray_unit_length * (1 - (position_x - (float) current_cell_x));
            step_x = 1;

        } else if (ray_dir_x < 0) { //we look at east
            x_ray_length += x_ray_unit_length * (position_x - (float) current_cell_x);
            step_x = -1;
        
        } else {
            step_x = 0;
        }

        if (ray_dir_y > 0) { // we look at south
            y_ray_length += y_ray_unit_length * (1 - (position_y - (float) current_cell_y));
            step_y = 1;
            
        } else if (ray_dir_y < 0) { //we look at north
            y_ray_length += y_ray_unit_length * (position_y - (float) current_cell_y);
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
                collision_side[r] = 'x';
            }
            else {
                ray_length = y_ray_length;
                y_ray_length += y_ray_unit_length;

                current_cell_y += step_y;
                collision_side[r] = 'y';
            }
            if (wall == (*map)[current_cell_x][current_cell_y]){
                break;
            }
        }

        vision_field[r+1].position = sf::Vector2f(
            (position_x + ray_length * ray_dir_x) * MINIMAP / 32.0,
            (position_y + ray_length * ray_dir_y) * MINIMAP / 32.0
        );

        // we don't use the real ray length to avoid fisheye effect
        if (collision_side[r] == 'x') {
            rays_lenght[r] = x_ray_length - x_ray_unit_length;
        } else {
            rays_lenght[r] = y_ray_length - y_ray_unit_length;
        }
    }

}

