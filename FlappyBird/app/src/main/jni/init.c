#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <android/native_window.h>
#include <android/asset_manager.h>
#include "init.h"
#include "utils.h"
#include "texture.h"
#include "shaders.h"
#include "audio.h"
#include "game.h"


bool                 g_Initialized = false;
EGLDisplay           g_EglDisplay = EGL_NO_DISPLAY;
EGLSurface           g_EglSurface = EGL_NO_SURFACE;
EGLContext           g_EglContext = EGL_NO_CONTEXT;
struct android_app* g_App = NULL;

int32_t WindowSizeX = 0;
int32_t WindowSizeY = 0;

GLuint textureProgram;

GLuint colorProgram;
GLuint gPositionHandle;
GLuint gColorHandle;

//fix by Tempa
const char* vertexShaderTexture =
    "attribute vec4 aPosition;\n"
    "attribute vec2 aTexCoord;\n"
    "varying vec2 vTexCoord;\n"
    "void main() {\n"
    "    gl_Position = aPosition;\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

const char* fragmentShaderTexture =
    "precision mediump float;\n"
    "varying vec2 vTexCoord;\n"
    "uniform sampler2D uTexture;\n"
    "void main() {\n"
    "    vec4 texColor = texture2D(uTexture, vTexCoord);\n"
    "    if (texColor.rgb == vec3(0.0)) {\n"
    "        texColor.a = 0.0;\n"
    "    }\n"
    "    gl_FragColor = texColor;\n"
    "}\n";

//by vadim
const char* vertexShaderColor =
    "attribute vec4 a_Position;\n"
    "void main() {\n"
    "    gl_Position = a_Position;\n"
    "}\n";

const char* fragmentShaderColor =
    "precision mediump float;\n"
    "uniform vec4 u_Color;\n"
    "void main() {\n"
    "    gl_FragColor = u_Color;\n"
    "}\n";


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

    if (!InitGame())
    {
        Log("Game not init!");
        return;
    }

    CreateAudioEngine();

    // Create shader program
    textureProgram = createProgram(vertexShaderTexture, fragmentShaderTexture);

    colorProgram = createProgram(vertexShaderColor, fragmentShaderColor);
    gPositionHandle = glGetAttribLocation(colorProgram, "a_Position");
    gColorHandle = glGetUniformLocation(colorProgram, "u_Color");


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

    Render();

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

    ShutdownGame();
    glDeleteProgram(textureProgram);
    glDeleteProgram(colorProgram);

    g_Initialized = false;
}