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
} Ressources;

# endif