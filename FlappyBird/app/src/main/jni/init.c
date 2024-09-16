#include "init.h"
#include "utils.h"
#include "texture.h"
#include "shaders.h"
#include "audio.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>
#include <android/asset_manager.h>

bool                 g_Initialized = false;
EGLDisplay           g_EglDisplay = EGL_NO_DISPLAY;
EGLSurface           g_EglSurface = EGL_NO_SURFACE;
EGLContext           g_EglContext = EGL_NO_CONTEXT;
struct android_app* g_App = NULL;

int32_t WindowSizeX = 0;
int32_t WindowSizeY = 0;

GLuint program;
GLuint texture;

void Init(struct android_app* app)
{
    if (g_Initialized)
        return;

    g_App = app;
    ANativeWindow_acquire(g_App->window);

    // Initialize EGL
    g_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_EglDisplay == EGL_NO_DISPLAY)
    {
        Log("eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");
        return;
    }

    if (eglInitialize(g_EglDisplay, 0, 0) != EGL_TRUE)
    {
        Log("eglInitialize() returned with an error");
        return;
    }

    const EGLint egl_attributes[] = {
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
    EGLint num_configs = 0;
    if (eglChooseConfig(g_EglDisplay, egl_attributes, NULL, 0, &num_configs) != EGL_TRUE)
    {
        Log("eglChooseConfig() returned with an error");
        return;
    }
    if (num_configs == 0)
    {
        Log("eglChooseConfig() returned 0 matching config");
        return;
    }

    EGLConfig egl_config;
    eglChooseConfig(g_EglDisplay, egl_attributes, &egl_config, 1, &num_configs);
    EGLint egl_format;
    eglGetConfigAttrib(g_EglDisplay, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(g_App->window, 0, 0, egl_format);

    const EGLint egl_context_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT, egl_context_attributes);
    if (g_EglContext == EGL_NO_CONTEXT)
    {
        Log("eglCreateContext() returned EGL_NO_CONTEXT");
        return;
    }

    g_EglSurface = eglCreateWindowSurface(g_EglDisplay, egl_config, g_App->window, NULL);
    if (g_EglSurface == EGL_NO_SURFACE)
    {
        Log("eglCreateWindowSurface() returned EGL_NO_SURFACE");
        return;
    }

    if (eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext) != EGL_TRUE)
    {
        Log("eglMakeCurrent() returned with an error");
        return;
    }

    // Set window size
    WindowSizeX = ANativeWindow_getWidth(g_App->window);
    WindowSizeY = ANativeWindow_getHeight(g_App->window);

    // Load texture
    texture = LoadTexture("buttons/pause.png");
    if (!texture)
    {
        Log("Failed to load texture");
        return;
    }

    createEngine();
    createAudioPlayer("audio/point.mp3");

    // Create shader program
    program = createProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(program);

    Log("FlappyBird is loaded!");

    g_Initialized = true;
}

void MainLoopStep()
{
    if (g_EglDisplay == EGL_NO_DISPLAY)
        return;

    // Setup display size (every frame to accommodate for window resizing)
    glViewport(0, 0, (int)WindowSizeX, (int)WindowSizeY);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render texture
    RenderTexture(texture, 200, 200, 100, 100);

    eglSwapBuffers(g_EglDisplay, g_EglSurface);
}

void Shutdown()
{
    if (!g_Initialized)
        return;

    // Cleanup
    if (g_EglDisplay != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(g_EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (g_EglContext != EGL_NO_CONTEXT)
            eglDestroyContext(g_EglDisplay, g_EglContext);

        if (g_EglSurface != EGL_NO_SURFACE)
            eglDestroySurface(g_EglDisplay, g_EglSurface);

        eglTerminate(g_EglDisplay);
    }

    g_EglDisplay = EGL_NO_DISPLAY;
    g_EglContext = EGL_NO_CONTEXT;
    g_EglSurface = EGL_NO_SURFACE;
    ANativeWindow_release(g_App->window);

    // Delete texture and program
    glDeleteTextures(1, &texture);
    glDeleteProgram(program);

    g_Initialized = false;
}