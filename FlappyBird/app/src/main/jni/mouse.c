#include "mouse.h"

void mouse_init(MouseState* mouse) 
{
    mouse->x = 0.0f;
    mouse->y = 0.0f;
    mouse->isDown = false;
    mouse->isReleased = false;
    mouse->isMoved = false;
}

void mouse_update(MouseState* mouse, float x, float y, bool down, bool released, bool moved) 
{
    mouse->x = x;
    mouse->y = y;
    mouse->isDown = down;
    mouse->isReleased = released;
    mouse->isMoved = moved;
}

void mouse_reset(MouseState* mouse) 
{
    mouse->isDown = false;
    mouse->isReleased = false;
    mouse->isMoved = false;
}