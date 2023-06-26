#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <SFML/Audio.h>
#include <SFML/Network.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH   800
#define HEIGHT  800

#define N_CIRCLES   1000
#define R   4

#define g 10;
#define GET_X(circle_ptr)    (sfCircleShape_getPosition((circle_ptr)->shape).x)
#define GET_Y(circle_ptr)    (sfCircleShape_getPosition((circle_ptr)->shape).y)
#define GET_R(circle_ptr)    (sfCircleShape_getRadius((circle_ptr)->shape))

typedef struct circle_ {
    float vx;
    float vy;
    sfCircleShape* shape;
} circle_t;


void border_collision(circle_t* c) {
    float dt = 0.1;
    sfCircleShape_move(c->shape, (sfVector2f) {c->vx*dt, c->vy*dt});
    // c->vy += 10*dt;
    if (GET_X(c) < 0) {
        c->vx = -c->vx;
        sfCircleShape_setPosition(c->shape, (sfVector2f) {0, GET_Y(c)});
    } else if (GET_X(c) > WIDTH - 2*GET_R(c)) {
        c->vx = -c->vx;
        sfCircleShape_setPosition(c->shape, (sfVector2f) {WIDTH - 2*GET_R(c), GET_Y(c)});
    }

    if (GET_Y(c) < 0) {
        c->vy = -c->vy;
        sfCircleShape_setPosition(c->shape, (sfVector2f) {GET_X(c), 0});
    } else if (GET_Y(c) > HEIGHT - 2*GET_R(c)) {
        c->vy = -c->vy;
        sfCircleShape_setPosition(c->shape, (sfVector2f) {GET_X(c), WIDTH - 2*GET_R(c)});
    }   
}

void collision(circle_t* c, int n_circles) {
    float d2, d, tmp;
    float dx;
    float dy;
    float v1x, v1y, v2x, v2y;
    for (int i=0; i<n_circles; i++) {
        for (int j=0; j<n_circles; j++) {
            if (i==j) continue;
            dx = (float) GET_X(c+i)-GET_X(c+j);
            dy = (float) GET_Y(c+i)-GET_Y(c+j);
            d2 = dx*dx + dy*dy;
            if (d2 < 4*R*R) {
                sfCircleShape_setFillColor(c[i].shape, sfBlue);
                sfCircleShape_setFillColor(c[j].shape, sfBlue);
                d = sqrtf(d2);

                v1x = dx*(dx*c[j].vx + dy*c[j].vy) + dy*(dy*c[i].vx - dx*c[i].vy);
                v1y = dy*(dx*c[j].vx + dy*c[j].vy) - dx*(dy*c[i].vx - dx*c[i].vy);
                v2x = dx*(dx*c[i].vx + dy*c[i].vy) + dy*(dy*c[j].vx - dx*c[j].vy);
                v2y = dy*(dx*c[i].vx + dy*c[i].vy) - dx*(dy*c[j].vx - dx*c[j].vy);
                c[i].vx = v1x/(d*d);
                c[i].vy = v1y/(d*d);
                c[j].vx = v2x/(d*d);
                c[j].vy = v2y/(d*d);
                
                tmp = 0.5f*d-R;
                sfCircleShape_move(c[i].shape, (sfVector2f) {-dx/d*tmp, -dy/d*tmp});
                sfCircleShape_move(c[j].shape, (sfVector2f) {dx/d*tmp, dy/d*tmp});

            }
        }
    }
}


int main() {
    srand(11);
    sfRenderWindow* window;
    sfVector2u size = {WIDTH, HEIGHT};
    sfVideoMode mode = {WIDTH, HEIGHT, 32};
    window = sfRenderWindow_create(mode, "Balls", sfTitlebar | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 60);


    circle_t circles[N_CIRCLES];

    for (int i=0; i<N_CIRCLES; i++) {
        sfCircleShape* c = sfCircleShape_create();
        sfCircleShape_setRadius(c, R);
        sfCircleShape_setFillColor(c, sfRed);
        sfCircleShape_setPosition(c, (sfVector2f) {rand()%WIDTH, rand()%HEIGHT});
        circles[i].shape = c;
        circles[i].vx = rand()%50;
        circles[i].vy = rand()%50;
    }


    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {   
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event.type == sfEvtKeyPressed) {
                switch (event.key.code) {
                case sfKeyRight:
                    printf("right\n");
                    break;
                
                default:
                    break;
                }
            }
        }

        if (sfRenderWindow_getSize(window).x != WIDTH || sfRenderWindow_getSize(window).y != HEIGHT) {
            sfRenderWindow_setSize(window, size);
        }
        
        collision(circles, N_CIRCLES);
        for (int i=0; i<N_CIRCLES; i++) {
            sfCircleShape_setFillColor(circles[i].shape, sfRed);
            border_collision(&circles[i]);
        }

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