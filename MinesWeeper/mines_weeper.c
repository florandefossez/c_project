#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/Audio.h>
#include <SFML/Network.h>
#include <stdio.h>
#include <stdlib.h>
#include "mines_weeper.h"

#define WIDTH 640
#define HEIGHT 640

void event_handler(sfMouseButtonEvent event, Game* game);
void set_up(Game* game);
void set_up_sprites(Ressources* ressources);
void draw_grid(Game* game, Ressources* Ressources);
void reveal(int row, int col, Game* game);
void set_flag(int row, int col, Game* game);
// void end(Game* game, bool win);
void free_ressources(Ressources* ressources);



int main()
{   
    Game game;
    set_up(&game);
    sfVector2u size = {WIDTH, HEIGHT};
    sfVideoMode mode = {WIDTH, HEIGHT, 32};
    game.window = sfRenderWindow_create(mode, "Mines Weeper", sfTitlebar | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(game.window, 60);

    Ressources ressources;
    set_up_sprites(&ressources);

    while (sfRenderWindow_isOpen(game.window))
    {   
        sfEvent event;
        while (sfRenderWindow_pollEvent(game.window, &event))
        {   
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(game.window);
            if (event.type == sfEvtMouseButtonPressed) {
                event_handler(event.mouseButton, &game);
            }
        }

        if (sfRenderWindow_getSize(game.window).x != WIDTH || sfRenderWindow_getSize(game.window).y != HEIGHT) {
            sfRenderWindow_setSize(game.window, size);
        }

        sfRenderWindow_clear(game.window, game.color);
        draw_grid(&game, &ressources);
        if (game.display_button) {
            sfRenderWindow_drawSprite(game.window, ressources.button, NULL);
            if (game.win) {
                sfRenderWindow_drawText(game.window, ressources.win, NULL);
            } else {
                sfRenderWindow_drawText(game.window, ressources.loose, NULL);
            }           
        }
        sfRenderWindow_display(game.window);
    }

    // sfTexture_destroy(texture);
    free_ressources(&ressources);
    sfRenderWindow_destroy(game.window);

    return 0;
}



void set_up(Game* game) {
    game->display_button = false;
    game->n_left_cells = 20*20;
    game->color = sfColor_fromRGBA(51, 28, 23, 255);

    for (int i=0; i<20; i++) {
        for (int j=0; j<20; j++) {
            game->grid[i][j].is_mine = false;
            game->grid[i][j].flag = false;
            game->grid[i][j].revealed = false;
            game->grid[i][j].surrounding_mines = 0;
        }
    }
    game->n_mines = 40;
    int i = game->n_mines;
    while (i > 0) {
        int l = rand()%20;
        int c = rand()%20;
        if (!game->grid[l][c].is_mine) {
            i--;
            game->grid[l][c].is_mine = true;
            for (int i=-1; i<2; i++) {
                for (int j=-1; j<2; j++) {
                    if (l+i<0 || c+j<0 || l+i>19 || c+j>19)
                        continue;
                    game->grid[l+i][c+j].surrounding_mines += 1;
                }
            }
        }
    }
}

void set_up_sprites(Ressources* ressources) {
    ressources->texture_game = sfTexture_createFromFile("minesweeper.png", NULL);
    ressources->texture_button = sfTexture_createFromFile("button.png", NULL);

    sfVector2f scale = {2,2};
    for (int i=0; i<8; i++) {
        ressources->sprite_numbers[i] = sfSprite_create();
        sfSprite_setTexture(ressources->sprite_numbers[i], ressources->texture_game, sfTrue);
        sfIntRect rect = {16*(i%4),16+16*(i/4),16,16};
        sfSprite_setTextureRect(ressources->sprite_numbers[i], rect);
        sfSprite_setScale(ressources->sprite_numbers[i], scale);
    }

    ressources->bomb = sfSprite_create();
    sfSprite_setTexture(ressources->bomb, ressources->texture_game, sfTrue);
    sfIntRect rect1 = {0,48,16,16};
    sfSprite_setTextureRect(ressources->bomb, rect1);
    sfSprite_setScale(ressources->bomb, scale);

    ressources->masked = sfSprite_create();
    sfSprite_setTexture(ressources->masked, ressources->texture_game, sfTrue);
    sfIntRect rect2 = {32,48,16,16};
    sfSprite_setTextureRect(ressources->masked, rect2);
    sfSprite_setScale(ressources->masked, scale);

    ressources->flag = sfSprite_create();
    sfSprite_setTexture(ressources->flag, ressources->texture_game, sfTrue);
    sfIntRect rect3 = {32,0,16,16};
    sfSprite_setTextureRect(ressources->flag, rect3);
    sfSprite_setScale(ressources->flag, scale);

    ressources->button = sfSprite_create();
    sfSprite_setTexture(ressources->button, ressources->texture_button, sfTrue);
    sfSprite_setScale(ressources->button, (sfVector2f) {2.f, 2.f});
    sfSprite_setPosition(ressources->button, (sfVector2f){(float)(WIDTH/2 - 96), 200.f});


    sfFont* font = sfFont_createFromFile("arial.ttf");
    sfColor fontColor = {246, 205, 38, 255};

    ressources->win = sfText_create();
    sfText_setString(ressources->win, "You Win !");
    sfText_setFont(ressources->win, font);
    sfText_setCharacterSize(ressources->win, 24);
    sfText_setColor(ressources->win, fontColor);
    sfFloatRect textBounds = sfText_getLocalBounds(ressources->win);
    sfText_setPosition(ressources->win, (sfVector2f) {
        WIDTH/2 - textBounds.width /2.f,
        232.f - textBounds.height
    });

    ressources->loose = sfText_create();
    sfText_setString(ressources->loose, "You Loose :(");
    sfText_setFont(ressources->loose, font);
    sfText_setCharacterSize(ressources->loose, 24);
    sfText_setColor(ressources->loose, fontColor);
    textBounds = sfText_getLocalBounds(ressources->loose);
    sfText_setPosition(ressources->loose, (sfVector2f) {
        WIDTH/2 - textBounds.width / 2.f,
        232.f - textBounds.height
    });
}

void draw_grid(Game* game, Ressources* ressources) {
    sfSprite* sprite;
    for (int i = 0; i<20; i++) {
        for (int j=0; j<20; j++) {
            sfVector2f position = {(float)(i*32), (float)(j*32)};
            if (game->grid[i][j].revealed) {
                if (game->grid[i][j].surrounding_mines > 0) {
                    if (game->grid[i][j].is_mine) {
                        sprite = ressources->bomb;
                    } else {
                        sprite = ressources->sprite_numbers[game->grid[i][j].surrounding_mines - 1];
                    } 
                } else {
                    continue;
                }
            } else {
                sprite = ressources->masked;
            }
            sfSprite_setPosition(sprite, position);
            sfRenderWindow_drawSprite(game->window, sprite, NULL);
            if (game->grid[i][j].flag) {
                sprite = ressources->flag;
                sfSprite_setPosition(sprite, position);
                sfRenderWindow_drawSprite(game->window, sprite, NULL);
            }
        }
    }
}

void event_handler(sfMouseButtonEvent event, Game* game) {
    if (game->display_button) {
        set_up(game);
        return;
    }
    if (event.button == sfMouseLeft) {
        reveal(event.x/32, event.y/32, game);
    } else if (event.button == sfMouseRight) {
        set_flag(event.x/32, event.y/32, game);
    }
}

void reveal(int row, int col, Game* game) {
    if (game->grid[row][col].revealed || game->grid[row][col].flag) {return;}
    game->grid[row][col].revealed = true;

    if (game->grid[row][col].is_mine) {
        game->win = false;
        game->display_button = true;
        return;
    }

    game->n_left_cells -= 1;

    if (game->n_left_cells == game->n_mines) {
        game->win = true;
        game->display_button = true;
    }

    if (game->grid[row][col].surrounding_mines == 0) {
        for (int i=-1; i<2; i++) {
            for (int j=-1; j<2; j++) {
                if (row+i<0 || col+j<0 || row+i>19 || col+j>19)
                    continue;
                reveal(i+row,j+col, game);
            }
        }
    }
}

void set_flag(int row, int col, Game* game) {
    if (!game->grid[row][col].revealed) {
        if (game->grid[row][col].flag) {
            game->grid[row][col].flag = false;
        } else {
            game->grid[row][col].flag = true;
        }
    }
}

void free_ressources(Ressources* ressources) {
    for (int i=0; i<8; i++) {
        sfSprite_destroy(ressources->sprite_numbers[i]);
    }
}
