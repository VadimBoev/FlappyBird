#ifndef MOUSE_H
#define MOUSE_H

#include <stdbool.h>

typedef struct {
    float x;
    float y;
    bool isDown;
    bool isReleased;
    bool isMoved;
} MouseState;

extern MouseState mouse;

void MouseInit(MouseState* mouse);
void MouseReset(MouseState* mouse);
bool IsMouseInSquare(int mouse_x, int mouse_y, int x, int y, int w, int h);

#endif // MOUSE_H