#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <SFML/Graphics.h>
#include <stdbool.h>


typedef struct {
    bool is_mine;
    bool flag;
    int surrounding_mines;
} Grid;


# endif