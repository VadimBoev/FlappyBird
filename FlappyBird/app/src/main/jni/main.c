#include "android_native_app_glue.h"
#include "init.h"
#include "utils.h"
#include "mouse.h"


#define TARGET_FPS 60
#define TARGET_FRAME_TIME (1.0f / TARGET_FPS)

double g_Time = 0.0;
float DeltaTime = 0.0f;
double g_LastFrameTime = 0.0;


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

int32_t handle_input(struct android_app* app, AInputEvent* event)
{
    int32_t eventType = AInputEvent_getType(event);

    if (eventType == AINPUT_EVENT_TYPE_MOTION) 
    {
        int32_t action = AMotionEvent_getAction(event);
        action &= AMOTION_EVENT_ACTION_MASK;
        int whichsource = action >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        size_t pointerCount = AMotionEvent_getPointerCount(event);
        float x = 0.0f;
        float y = 0.0f;
        bool isDown = false;
        bool isReleased = false;
        bool isMoved = false;
        int index;

        for (size_t i = 0; i < pointerCount; ++i)
        {
            x = AMotionEvent_getX(event, i);
            y = AMotionEvent_getY(event, i);
            index = AMotionEvent_getPointerId(event, i);

            if (action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_DOWN)
            {
                int id = index;
                if (action == AMOTION_EVENT_ACTION_POINTER_DOWN && id != whichsource) continue;

                isDown = true;
            }
            else if (action == AMOTION_EVENT_ACTION_POINTER_UP || action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_CANCEL)
            {
                int id = index;
                if (action == AMOTION_EVENT_ACTION_POINTER_UP && id != whichsource) continue;

                isReleased = true;
            }
            else if (action == AMOTION_EVENT_ACTION_MOVE)
            {
                isMoved = true;
            }
        }

        MouseUpdate(&mouse, x, y, isDown, isReleased, isMoved);

        return 1;
    }
    //else if (eventType == AINPUT_EVENT_TYPE_KEY) //Apparently, this applies to physical keys, including volume control keys.
    //{
    //    return 0;
    //}
    return 0; //Return 0 if you are not processing the event
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
            struct timespec current_timespec;
            clock_gettime(CLOCK_MONOTONIC, &current_timespec);
            double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);

            // calc delta time
            DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);

            // checking if enough time has passed for a new frame
            if (current_time - g_LastFrameTime >= TARGET_FRAME_TIME)
            {
                MainLoopStep();
                MouseReset(&mouse);

                // update time last frame
                g_LastFrameTime = current_time;
            }

            g_Time = current_time;
        }
    }
}