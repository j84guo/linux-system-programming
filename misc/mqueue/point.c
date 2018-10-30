#include "point.h"

void set_position(int x, int y, struct point *buf)
{
    buf->x = x;
    buf->y = y;
}

int get_x_coord(struct point buf)
{
    return buf.x;
}

int get_y_coord(struct point buf)
{
    return buf.y;
}
