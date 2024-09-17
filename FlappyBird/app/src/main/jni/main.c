#include "android_native_app_glue.h"
#include "init.h"
#include "utils.h"
#include "mouse.h"

static void handleAppCmd(struct android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        Init(app);
        break;
    case APP_CMD_TERM_WINDOW:
        Shutdown();
        break;
    case APP_CMD_GAINED_FOCUS:
        break;
    case APP_CMD_LOST_FOCUS:
        break;
    }
}

void handle_input(struct android_app* app, AInputEvent* event) 
{
    int32_t eventType = AInputEvent_getType(event);

    if (eventType == AINPUT_EVENT_TYPE_MOTION) 
    {
        int32_t action = AMotionEvent_getAction(event);
        int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int32_t pointerId = AMotionEvent_getPointerId(event, pointerIndex);
        float x = AMotionEvent_getX(event, pointerIndex);
        float y = AMotionEvent_getY(event, pointerIndex);
        bool isDown = false;
        bool isReleased = false;
        bool isMoved = false;

        switch (action & AMOTION_EVENT_ACTION_MASK) 
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            isDown = true;
            //Log("Touch down at (%f, %f)", x, y);
            break;

        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            isReleased = true;
            //Log("Touch up at (%f, %f)", x, y);
            break;

        case AMOTION_EVENT_ACTION_MOVE:
            isMoved = true;
            //Log("Touch move at (%f, %f)", x, y);
            break;
        }

        MouseUpdate(&mouse, x, y, isDown, isReleased, isMoved);
    }
}


void android_main(struct android_app* state)
{
    state->onInputEvent = handle_input;
    state->onAppCmd = handleAppCmd;

    MouseInit(&mouse);

    while (1)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
        {
            if (source != NULL) {
                source->process(state, source);
            }

            if (state->destroyRequested != 0) {
                Shutdown();
                return;
            }
        }

        if (g_Initialized) 
        {
            MainLoopStep();
            MouseReset(&mouse);
        }
    }
}