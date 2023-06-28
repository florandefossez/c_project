#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH   800
#define HEIGHT  800

#define N_CIRCLES   1000
#define R   5

#define g 10;
#define GET_X(circle_ptr)    (sfCircleShape_getPosition((circle_ptr)->shape).x)
#define GET_Y(circle_ptr)    (sfCircleShape_getPosition((circle_ptr)->shape).y)
#define GET_R(circle_ptr)    (sfCircleShape_getRadius((circle_ptr)->shape))

typedef struct circle_ {
    float vx;
    float vy;
    sfCircleShape* shape;
} circle_t;


void update(circle_t* c, int n_circles) {
    float dt = 0.1;
    for (int i=0; i<n_circles; i++) {
        sfCircleShape_setFillColor(c[i].shape, sfWhite);
        sfCircleShape_move(c[i].shape, (sfVector2f) {c[i].vx*dt, c[i].vy*dt});
        c[i].vy += 10*dt;
    }
}

void collision(circle_t* c, int n_circles) {
    float d2, d, tmp;
    float dx;
    float dy;
    float v1x, v1y, v2x, v2y;
    for (int i=0; i<n_circles; i++) {
        if (GET_X(c+i) < 0) {
            c[i].vx = -c[i].vx;
            sfCircleShape_setPosition(c[i].shape, (sfVector2f) {0, GET_Y(c+i)});
        } else if (GET_X(c+i) > WIDTH -2*R) {
            c[i].vx = -c[i].vx;
            sfCircleShape_setPosition(c[i].shape, (sfVector2f) {WIDTH -2*R, GET_Y(c+i)});
        }

        if (GET_Y(c+i) < 0) {
            c[i].vy = -c[i].vy;
            sfCircleShape_setPosition(c[i].shape, (sfVector2f) {GET_X(c+i), 0});
        } else if (GET_Y(c+i) > HEIGHT -2*R) {
            c[i].vy = -c[i].vy;
            sfCircleShape_setPosition(c[i].shape, (sfVector2f) {GET_X(c+i), HEIGHT -2*R});
        }   
        for (int j=0; j<n_circles; j++) {
            if (i==j) continue;
            dx = (float) GET_X(c+i)-GET_X(c+j);
            dy = (float) GET_Y(c+i)-GET_Y(c+j);
            d2 = dx*dx + dy*dy;
            if (d2 < 4*R*R) {
                sfCircleShape_setFillColor(c[i].shape, sfRed);
                sfCircleShape_setFillColor(c[j].shape, sfRed);
                d = sqrtf(d2);

                v1x = dx*(dx*c[j].vx + dy*c[j].vy) + dy*(dy*c[i].vx - dx*c[i].vy);
                v1y = dy*(dx*c[j].vx + dy*c[j].vy) - dx*(dy*c[i].vx - dx*c[i].vy);
                v2x = dx*(dx*c[i].vx + dy*c[i].vy) + dy*(dy*c[j].vx - dx*c[j].vy);
                v2y = dy*(dx*c[i].vx + dy*c[i].vy) - dx*(dy*c[j].vx - dx*c[j].vy);
                c[i].vx = v1x/d2;
                c[i].vy = v1y/d2;
                c[j].vx = v2x/d2;
                c[j].vy = v2y/d2;
                
                tmp = 0.5f*d-R;
                sfCircleShape_move(c[i].shape, (sfVector2f) {-dx/d*tmp, -dy/d*tmp});
                sfCircleShape_move(c[j].shape, (sfVector2f) {dx/d*tmp, dy/d*tmp});

            }
        }
    }
}


int main() {
    sfRenderWindow* window;
    sfVector2u size = {WIDTH, HEIGHT};
    sfVideoMode mode = {WIDTH, HEIGHT, 32};
    window = sfRenderWindow_create(mode, "Balls", sfTitlebar | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 40);

    circle_t circles[N_CIRCLES];
    for (int i=0; i<N_CIRCLES; i++) {
        sfCircleShape* c = sfCircleShape_create();
        sfCircleShape_setRadius(c, R);
        sfCircleShape_setFillColor(c, sfWhite);
        sfCircleShape_setPosition(c, (sfVector2f) {rand()%WIDTH, rand()%HEIGHT});
        circles[i].shape = c;
        circles[i].vx = rand()%50 - 25;
        circles[i].vy = rand()%50 - 25;
    }


    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {   
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
        }

        if (sfRenderWindow_getSize(window).x != WIDTH || sfRenderWindow_getSize(window).y != HEIGHT) {
            sfRenderWindow_setSize(window, size);
        }
        
        update(circles, N_CIRCLES);
        collision(circles, N_CIRCLES);
        collision(circles, N_CIRCLES);
        collision(circles, N_CIRCLES);
   
        sfRenderWindow_clear(window, sfBlack);
        for (int i=0; i<N_CIRCLES; i++) {
            sfRenderWindow_drawCircleShape(window, circles[i].shape, NULL);
        }
        sfRenderWindow_display(window);
    }

    sfRenderWindow_destroy(window);

    return 0;
}