#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/Audio.h>
#include <SFML/Network.h>
#include <stdbool.h>


typedef struct {
    bool is_mine;
    bool flag;
    bool revealed;
    int surrounding_mines;
} Grid;

typedef struct {
    sfSprite* sprite_numbers[8];
    sfSprite* bomb;
    sfSprite* flag;
    sfSprite* masked;
    sfSprite* button;
    sfText* win;
    sfText* loose;
    sfTexture* texture_button;
    sfTexture* texture_game;
} Ressources;

typedef struct {
    Grid grid[20][20];
    bool display_button;
    bool win;
    int n_mines;
    int n_left_cells;
    sfRenderWindow* window;
    sfColor color;
} Game;

# endif