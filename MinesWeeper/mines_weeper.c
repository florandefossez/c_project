#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/Audio.h>
#include <SFML/Network.h>
#include <stdio.h>
#include <stdlib.h>
#include "mines_weeper.h"

#define WIDTH 640
#define HEIGHT 640

void event_handler(sfMouseButtonEvent event, Grid grid[20][20], int *left_cells);
void set_up(Grid grid[20][20], int n_mines);
void set_up_sprites(Ressources* ressources, sfTexture* texture);
void draw_grid(Ressources* Ressources, Grid grid[20][20], sfRenderWindow* window);
void reveal(int row, int col, Grid grid[20][20], int *left_cells);
void set_flag(int row, int col, Grid grid[20][20]);
void free_ressources(Ressources* ressources);



int main()
{   
    int n_mines = 20;
    int left_cells = 20*20;
    sfVector2u size = {WIDTH, HEIGHT};
    sfVideoMode mode = {WIDTH, HEIGHT, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "Mines Weeper", sfTitlebar | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 60);
    

    Grid grid[20][20];

    set_up(grid, n_mines);

    sfTexture* texture = sfTexture_createFromFile("minesweeper.png", NULL);
    if (texture == NULL) {
        printf("Failed to load texture");
        return 1;
    }
    Ressources ressources;
    set_up_sprites(&ressources, texture);

    sfColor background_color = {51, 28, 23, 255};


    while (sfRenderWindow_isOpen(window))
    {   
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {   
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event.type == sfEvtMouseButtonPressed) {
                event_handler(event.mouseButton, grid, &left_cells);
            }
        }

        if (sfRenderWindow_getSize(window).x != WIDTH || sfRenderWindow_getSize(window).y != HEIGHT) {
            sfRenderWindow_setSize(window, size);
        }


        if (left_cells == n_mines) {
            printf("You win\n");
            set_up(grid, n_mines);
        }

        sfRenderWindow_clear(window, background_color);
        draw_grid(&ressources, grid, window);
        sfRenderWindow_display(window);
    }

    sfTexture_destroy(texture);
    free_ressources(&ressources);
    sfRenderWindow_destroy(window);

    return 0;
}



void set_up(Grid grid[20][20], int n_mines) {
    for (int i=0; i<20; i++) {
        for (int j=0; j<20; j++) {
            grid[i][j].is_mine = false;
            grid[i][j].flag = false;
            grid[i][j].revealed = false;
            grid[i][j].surrounding_mines = 0;
        }
    }

    while (n_mines > 0) {
        int l = rand()%20;
        int c = rand()%20;
        if (!grid[l][c].is_mine) {
            n_mines--;
            grid[l][c].is_mine = true;
            for (int i=-1; i<2; i++) {
                for (int j=-1; j<2; j++) {
                    if (l+i<0 || c+j<0 || l+i>19 || c+j>19)
                        continue;
                    grid[l+i][c+j].surrounding_mines += 1;
                }
            }
        }
    }
}

void set_up_sprites(Ressources* ressources, sfTexture* texture) {
    sfVector2f scale = {2,2};
    for (int i=0; i<8; i++) {
        ressources->sprite_numbers[i] = sfSprite_create();
        sfSprite_setTexture(ressources->sprite_numbers[i], texture, sfTrue);
        sfIntRect rect = {16*(i%4),16+16*(i/4),16,16};
        sfSprite_setTextureRect(ressources->sprite_numbers[i], rect);
        sfSprite_setScale(ressources->sprite_numbers[i], scale);
    }

    ressources->bomb = sfSprite_create();
    sfSprite_setTexture(ressources->bomb, texture, sfTrue);
    sfIntRect rect1 = {0,0,16,16};
    sfSprite_setTextureRect(ressources->bomb, rect1);
    sfSprite_setScale(ressources->bomb, scale);

    ressources->masked = sfSprite_create();
    sfSprite_setTexture(ressources->masked, texture, sfTrue);
    sfIntRect rect2 = {32,48,16,16};
    sfSprite_setTextureRect(ressources->masked, rect2);
    sfSprite_setScale(ressources->masked, scale);

    ressources->flag = sfSprite_create();
    sfSprite_setTexture(ressources->flag, texture, sfTrue);
    sfIntRect rect3 = {32,0,16,16};
    sfSprite_setTextureRect(ressources->flag, rect3);
    sfSprite_setScale(ressources->flag, scale);

}

void draw_grid(Ressources* ressources, Grid grid[20][20], sfRenderWindow* window) {
    sfSprite* sprite;
    for (int i = 0; i<20; i++) {
        for (int j=0; j<20; j++) {
            sfVector2f position = {(float)(i*32), (float)(j*32)};
            if (grid[i][j].revealed) {
                if (grid[i][j].surrounding_mines > 0) {
                    if (grid[i][j].is_mine) {
                        sprite = ressources->bomb;
                    } else {
                        sprite = ressources->sprite_numbers[grid[i][j].surrounding_mines - 1];
                    } 
                } else {
                    continue;
                }
            } else {
                sprite = ressources->masked;
            }
            sfSprite_setPosition(sprite, position);
            sfRenderWindow_drawSprite(window, sprite, NULL);
            if (grid[i][j].flag) {
                sprite = ressources->flag;
                sfSprite_setPosition(sprite, position);
                sfRenderWindow_drawSprite(window, sprite, NULL);
            }
        }
    }
}

void event_handler(sfMouseButtonEvent event, Grid grid[20][20], int *left_cells) {
    if (event.button == sfMouseLeft) {
        printf("clic gauche\n");
        reveal(event.x/32, event.y/32, grid, left_cells);
    } else if (event.button == sfMouseRight) {
        printf("clic droit\n");
        set_flag(event.x/32, event.y/32, grid);
    }
}

void reveal(int row, int col, Grid grid[20][20], int * left_cells) {

    if (grid[row][col].revealed || grid[row][col].flag) {return;}
    grid[row][col].revealed = true;

    if (grid[row][col].is_mine) {
        printf("You loose\n");
        return;
    }

    --(*left_cells);
    
    if (grid[row][col].surrounding_mines == 0) {
        for (int i=-1; i<2; i++) {
            for (int j=-1; j<2; j++) {
                if (row+i<0 || col+j<0 || row+i>19 || col+j>19)
                    continue;
                reveal(i+row,j+col,grid, left_cells);
            }
        }
    }
}

void set_flag(int row, int col, Grid grid[20][20]) {
    printf("in set_flag\n");
    if (!grid[row][col].revealed) {
        if (grid[row][col].flag) {
            grid[row][col].flag = false;
        } else {
            grid[row][col].flag = true;
        }
    }
}



void free_ressources(Ressources* ressources) {
    for (int i=0; i<8; i++) {
        sfSprite_destroy(ressources->sprite_numbers[i]);
    }
}
