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

void mouse_init(MouseState* mouse);
void mouse_update(MouseState* mouse, float x, float y, bool down, bool released, bool moved);
void mouse_reset(MouseState* mouse);

#endif // MOUSE_H