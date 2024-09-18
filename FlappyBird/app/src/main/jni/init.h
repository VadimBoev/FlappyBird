#ifndef INIT_H
#define INIT_H

#include "android_native_app_glue.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>

extern bool g_Initialized;
extern struct android_app* g_App;
extern int32_t WindowSizeX;
extern int32_t WindowSizeY;

extern GLuint textureProgram;

extern GLuint colorProgram;
extern GLuint gPositionHandle;
extern GLuint gColorHandle;


void Init(struct android_app* app);
void MainLoopStep();
void Shutdown();

#endif // INIT_H