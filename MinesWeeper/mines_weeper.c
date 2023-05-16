#include <SFML/Graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include "mines_weeper.h"

void event_handler(sfMouseButtonEvent event);
void set_up(Grid grid[20][20]);



int main()
{
    sfVideoMode mode = {600, 600, 32};
    sfRenderWindow* window = sfRenderWindow_create(mode, "Mines Weeper", sfClose, NULL);

    Grid grid[20][20];

    set_up(grid);

    while (sfRenderWindow_isOpen(window))
    {   

        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event.type == sfEvtMouseButtonPressed)
                event_handler(event.mouseButton);
        }

        sfRenderWindow_clear(window, sfWhite);
        sfRenderWindow_display(window);
    }


    sfRenderWindow_destroy(window);

    return 0;
}



void set_up(Grid grid[20][20]) {
    for (int i=0; i<20; i++) {
        for (int j=0; j<20; j++) {
            grid[i][j].is_mine = false;
            grid[i][j].flag = false;
            grid[i][j].surrounding_mines = 0;
        }
    }

    int n_mines = 12;
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

void event_handler(sfMouseButtonEvent event) {
    printf("%d %d\n", event.x, event.y);
}