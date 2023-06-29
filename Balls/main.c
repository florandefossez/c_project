#include <SFML/Graphics.h>
#include <SFML/Window.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH   800
#define HEIGHT  800
#define PAL 100

#define N_CIRCLES   500
#define R   5

#define g 10;
#define GET_X(circle)    (sfCircleShape_getPosition((circle).shape).x)
#define GET_Y(circle)    (sfCircleShape_getPosition((circle).shape).y)
#define GET_R(circle)    (sfCircleShape_getRadius((circle).shape))

int Nx = WIDTH/(2*R);
int Ny = HEIGHT/(2*R);

typedef struct circle_ {
    float vx;
    float vy;
    sfCircleShape* shape;
    int id;
} circle_t;


void update(circle_t* c, int n_circles) {
    float dt = 0.2;
    for (int i=0; i<n_circles; i++) {
        sfCircleShape_setFillColor(c[i].shape, sfWhite);
        sfCircleShape_move(c[i].shape, (sfVector2f) {c[i].vx*dt, c[i].vy*dt});
        c[i].vy += 10*dt;
    }
}


void collision(int partition[Nx][Ny][PAL], circle_t* c) {
    int k;
    for (int i=0; i<Nx; i++) {
        k=0;
        while (partition[i][Ny-1][k] != 0 && k<PAL){
            if (GET_Y(c[partition[i][Ny-1][k]-1])>HEIGHT) {
                sfCircleShape_setPosition(c[partition[i][Ny-1][k]-1].shape, (sfVector2f) {GET_X(c[partition[i][Ny-1][k]-1]), HEIGHT});
                c[partition[i][Ny-1][k]-1].vy *= -1;
            }
            k++;
        }
        k=0;
        while (partition[i][0][k] != 0 && k<PAL){
            if (GET_Y(c[partition[i][0][k]-1])<0) {
                sfCircleShape_setPosition(c[partition[i][0][k]-1].shape, (sfVector2f) {GET_X(c[partition[i][0][k]-1]), 0});
                c[partition[i][0][k]-1].vy *= -1;
            }
            k++;
        }
    }
    for (int i=0; i<Ny; i++) {
        k=0;
        while (partition[0][i][k] != 0 && k<PAL){
            if (GET_X(c[partition[0][i][k]-1])<0) {
                sfCircleShape_setPosition(c[partition[0][i][k]-1].shape, (sfVector2f) {0, GET_Y(c[partition[0][i][k]-1])});
                c[partition[0][i][k]-1].vx *= -1;
            }
            k++;
        }
        k=0;
        while (partition[Nx-1][i][k] != 0 && k<PAL){
            if (GET_X(c[partition[Nx-1][i][k]-1])>WIDTH) {
                sfCircleShape_setPosition(c[partition[Nx-1][i][k]-1].shape, (sfVector2f) {WIDTH, GET_Y(c[partition[Nx-1][i][k]-1])});
                c[partition[Nx-1][i][k]-1].vx *= -1;
            }
            k++;
        }
    }

    int local_circles[PAL*20];
    int len;

    float dx, dy, d2, d, tmp;
    int v1x, v2x, v1y, v2y;

    for (int i=1; i<Nx-1; i++) {
        for (int j=1; j<Ny-1; j++) {
            len = 0;
            for (int ii=i-1; ii<i+2; ii++) {
                for (int jj=j-1; jj<j+2; jj++) {
                    k = 0;
                    while (partition[ii][jj][k] != 0 && k<PAL) {
                        local_circles[len] = partition[ii][jj][k] - 1;
                        len++;
                        k++;
                    }
                }
            }


            for (int ii=0; ii<len; ii++) {
                for (int jj=0; jj<len; jj++) {
                    if (ii==jj) continue;
                    dx = (float) GET_X(c[local_circles[ii]])-GET_X(c[local_circles[jj]]);
                    dy = (float) GET_Y(c[local_circles[ii]])-GET_Y(c[local_circles[jj]]);
                    d2 = dx*dx + dy*dy;
                    if (d2 < 4*R*R) {
                        sfCircleShape_setFillColor(c[local_circles[ii]].shape, sfRed);
                        sfCircleShape_setFillColor(c[local_circles[jj]].shape, sfRed);
                        d = sqrtf(d2);

                        v1x = dx*(dx*c[local_circles[jj]].vx + dy*c[local_circles[jj]].vy) + dy*(dy*c[local_circles[ii]].vx - dx*c[local_circles[ii]].vy);
                        v1y = dy*(dx*c[local_circles[jj]].vx + dy*c[local_circles[jj]].vy) - dx*(dy*c[local_circles[ii]].vx - dx*c[local_circles[ii]].vy);
                        v2x = dx*(dx*c[local_circles[ii]].vx + dy*c[local_circles[ii]].vy) + dy*(dy*c[local_circles[jj]].vx - dx*c[local_circles[jj]].vy);
                        v2y = dy*(dx*c[local_circles[ii]].vx + dy*c[local_circles[ii]].vy) - dx*(dy*c[local_circles[jj]].vx - dx*c[local_circles[jj]].vy);
                        c[local_circles[ii]].vx = v1x/d2;
                        c[local_circles[ii]].vy = v1y/d2;
                        c[local_circles[jj]].vx = v2x/d2;
                        c[local_circles[jj]].vy = v2y/d2;
                        
                        tmp = 0.5f*d-R;
                        sfCircleShape_move(c[local_circles[ii]].shape, (sfVector2f) {-dx/d*tmp, -dy/d*tmp});
                        sfCircleShape_move(c[local_circles[jj]].shape, (sfVector2f) {dx/d*tmp, dy/d*tmp});
                    }
                }
            }
        }
    }
}

void set_partition(int partition[Nx][Ny][PAL], circle_t* c) {
    int k;
    int x;
    int y;
    for (int i=0; i<Nx; i++) {
        for (int j=0; j<Ny; j++) {
            for (k=0; k<PAL; k++) {
                partition[i][j][k] = 0;
            }
        }
    }
    int *part;
    for (int i=0; i<N_CIRCLES; i++) {
        x = (int) (GET_X(c[i])/(float)(2*R));
        y = (int) (GET_Y(c[i])/(float)(2*R));
        if (x>=Nx) x = Nx-1;
        if (x<0) x = 0;
        if (y>=Ny) y = Ny-1;
        if (y<0) y = 0;
        part = partition[x][y];
        k = 0;
        while (part[k] != 0 && k<PAL) {
            k++;
        }
        if (k==PAL) {
            printf("Warning: more than PAL balls per cell %d\n", i+1);
        } else {
            part[k] = i+1;
        }
    }
}


int main() {
    sfRenderWindow* window;
    sfVector2u size = {WIDTH, HEIGHT};
    sfVideoMode mode = {WIDTH, HEIGHT, 32};
    window = sfRenderWindow_create(mode, "Balls", sfTitlebar | sfClose, NULL);
    sfRenderWindow_setFramerateLimit(window, 30);

    int partition[Nx][Ny][PAL];

    circle_t circles[N_CIRCLES];
    for (int i=0; i<N_CIRCLES; i++) {
        sfCircleShape* c = sfCircleShape_create();
        sfCircleShape_setRadius(c, R);
        sfCircleShape_setFillColor(c, sfWhite);
        sfCircleShape_setPosition(c, (sfVector2f) {rand()%WIDTH, rand()%HEIGHT});
        circles[i].shape = c;
        circles[i].vx = rand()%50 - 25;
        circles[i].vy = rand()%50 - 25;
        circles[i].id = i+1;
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
        set_partition(partition, circles);
        collision(partition, circles);
        set_partition(partition, circles);
        collision(partition, circles);
        set_partition(partition, circles);
        collision(partition, circles);
        set_partition(partition, circles);
        collision(partition, circles);

        sfRenderWindow_clear(window, sfBlack);
        for (int i=0; i<N_CIRCLES; i++) {
            sfRenderWindow_drawCircleShape(window, circles[i].shape, NULL);
        }
        sfRenderWindow_display(window);
    }

    sfRenderWindow_destroy(window);

    return 0;
}