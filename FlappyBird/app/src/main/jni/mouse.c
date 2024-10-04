#include "mouse.h"
MouseState mouse;

void MouseInit(MouseState* mouse) 
{
    mouse->x = 0.0f;
    mouse->y = 0.0f;
    mouse->isDown = false;
    mouse->isReleased = false;
    mouse->isMoved = false;
}

void MouseReset(MouseState* mouse) 
{
    mouse->x = -1.0f;
    mouse->y = -1.0f;
    mouse->isDown = false;
    mouse->isReleased = false;
    mouse->isMoved = false;
}

bool IsMouseInSquare(int mouse_x, int mouse_y, int x, int y, int w, int h) 
{
    if (x <= mouse_x && mouse_x <= x + w &&
        y <= mouse_y && mouse_y <= y + h) 
    {
        return true;
    }
    else 
    {
        return false;
    }
}