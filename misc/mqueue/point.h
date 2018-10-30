#ifndef POINT_H
#define POINT_H

struct point {
    int x;
    int y;
};

void set_position(int x, int y, struct point *buf);

int get_x_coord(struct point p);
int get_y_coord(struct point p);

#endif
