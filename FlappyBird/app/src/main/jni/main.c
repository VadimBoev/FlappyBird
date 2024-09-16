#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES2/gl2.h>
#include "android_native_app_glue.h"
#include <android/log.h>
#include <jni.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "flappy", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "flappy", __VA_ARGS__))

// load math
#include <math.h>

// load png
#include "upng.h"

// audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// asset manager
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

// =================== Data ========================
// init OpenGL ES
static bool                 g_Initialized = false;
static EGLDisplay           g_EglDisplay = EGL_NO_DISPLAY;
static EGLSurface           g_EglSurface = EGL_NO_SURFACE;
static EGLContext           g_EglContext = EGL_NO_CONTEXT;
static struct android_app*  g_App = NULL;

// init OpenSL ES
SLObjectItf engineObject = NULL;
SLEngineItf engineEngine;
SLObjectItf outputMixObject = NULL;
SLObjectItf playerObject = NULL;
SLPlayItf playerPlay;
SLSeekItf playerSeek;

//=================================================

const char* vertexShaderSource =
    "attribute vec4 aPosition;"
    "attribute vec2 aTexCoord;"
    "varying vec2 vTexCoord;"
    "void main() {"
    "    gl_Position = aPosition;"
    "    vTexCoord = aTexCoord;"
    "}";

const char* fragmentShaderSource =
    "precision mediump float;"
    "varying vec2 vTexCoord;"
    "uniform sampler2D uTexture;"
    "void main() {"
    "    gl_FragColor = texture2D(uTexture, vTexCoord);"
    "}";

GLuint loadShader(GLenum type, const char* source) 
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource) 
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    return program;
}


void createEngine() 
{
    SLresult result;

    // create engine
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to create engine");
        return;
    }

    // init engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to realize engine");
        return;
    }

    // get interface engine
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to get engine interface");
        return;
    }

    // create output sound
    const SLInterfaceID ids[1] = { SL_IID_ENVIRONMENTALREVERB };
    const SLboolean req[1] = { SL_BOOLEAN_FALSE };
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if (result != SL_RESULT_SUCCESS)
    {
        LOGE("Failed to create output mix");
        return;
    }

    // init output
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to realize output mix");
        return;
    }
}

void createAudioPlayer(AAssetManager* assetManager, const char* assetPath) 
{
    SLresult result;

    AAsset* audioAsset = AAssetManager_open(assetManager, assetPath, AASSET_MODE_BUFFER);
    if (!audioAsset) 
    {
        LOGE("Failed to open asset file");
        return;
    }

    off_t start, length;
    int fd = AAsset_openFileDescriptor(audioAsset, &start, &length);
    AAsset_close(audioAsset);

    if (fd < 0) 
    {
        LOGE("Failed to get file descriptor");
        return;
    }

    // create source data
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_MP3 };
    SLDataSource audioSrc = { &loc_fd, &format_mime };

    // setting receive data
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
    SLDataSink audioSnk = { &loc_outmix, NULL };

    // create player
    const SLInterfaceID ids[3] = { SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME };
    const SLboolean req[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audioSrc, &audioSnk, 3, ids, req);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to create audio player");
        return;
    }

    // init player
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to realize player");
        return;
    }

    // get interface player
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to get play interface");
        return;
    }

    // get interface seek
    result = (*playerObject)->GetInterface(playerObject, SL_IID_SEEK, &playerSeek);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to get seek interface");
        return;
    }

    // go play
    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) 
    {
        LOGE("Failed to start playing");
        return;
    }
}

void pauseAudio() 
{
    SLresult result;
    if (playerPlay) 
    {
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PAUSED);
        if (result != SL_RESULT_SUCCESS) 
        {
            LOGE("Failed to pause playing");
        }
    }
}

void resumeAudio() 
{
    SLresult result;
    if (playerPlay) 
    {
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
        if (result != SL_RESULT_SUCCESS) 
        {
            LOGE("Failed to resume playing");
        }
    }
}

void stopAudio() 
{
    SLresult result;
    if (playerPlay) 
    {
        result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_STOPPED);
        if (result != SL_RESULT_SUCCESS) 
        {
            LOGE("Failed to stop playing");
        }
    }
}

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
        LOGE("eglGetDisplay(EGL_DEFAULT_DISPLAY) returned EGL_NO_DISPLAY");
        return;
    }

    if (eglInitialize(g_EglDisplay, 0, 0) != EGL_TRUE)
    {
        LOGE("eglInitialize() returned with an error");
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
    if (eglChooseConfig(g_EglDisplay, egl_attributes, NULL, 0, &num_configs) != EGL_TRUE) {
        LOGE("eglChooseConfig() returned with an error");
        return;
    }
    if (num_configs == 0) {
        LOGE("eglChooseConfig() returned 0 matching config");
        return;
    }

    EGLConfig egl_config;
    eglChooseConfig(g_EglDisplay, egl_attributes, &egl_config, 1, &num_configs);
    EGLint egl_format;
    eglGetConfigAttrib(g_EglDisplay, egl_config, EGL_NATIVE_VISUAL_ID, &egl_format);
    ANativeWindow_setBuffersGeometry(g_App->window, 0, 0, egl_format);

    const EGLint egl_context_attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    g_EglContext = eglCreateContext(g_EglDisplay, egl_config, EGL_NO_CONTEXT, egl_context_attributes);
    if (g_EglContext == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext() returned EGL_NO_CONTEXT");
        return;
    }

    g_EglSurface = eglCreateWindowSurface(g_EglDisplay, egl_config, g_App->window, NULL);
    if (g_EglSurface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface() returned EGL_NO_SURFACE");
        return;
    }

    if (eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, g_EglContext) != EGL_TRUE) {
        LOGE("eglMakeCurrent() returned with an error");
        return;
    }

    // Load and decode PNG
    unsigned char* buffer;
    unsigned long len_file = 0;

    AAsset* file = AAssetManager_open(g_App->activity->assetManager, "buttons/pause.png", AASSET_MODE_BUFFER);
    if (file)
    {
        LOGI("File buttons.png found");
        buffer = (unsigned char*)AAsset_getBuffer(file);
        len_file = AAsset_getLength(file);

        upng_t* png = upng_new_from_bytes(buffer, len_file);

        if (png == NULL) {
            LOGI("Error create PNG.\n");
            return;
        }

        upng_decode(png);

        if (upng_get_error(png) != UPNG_EOK)
        {
            LOGI("Error with decode PNG");
            upng_free(png);
            return;
        }

        unsigned width = upng_get_width(png);
        unsigned height = upng_get_height(png);
        unsigned bpp = upng_get_bpp(png);

        LOGI("Width: %u\n", width);
        LOGI("Height: %u\n", height);
        LOGI("Bit in pixel: %u\n", bpp);

        // Create texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Load data into texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, upng_get_buffer(png));

        upng_free(png);
    }

    createEngine();
    createAudioPlayer(g_App->activity->assetManager, "audio/point.mp3");

    // Create shader program
    program = createProgram(vertexShaderSource, fragmentShaderSource);
    glUseProgram(program);

    LOGE("FlappyBird is loaded!");

    g_Initialized = true;
}

void MainLoopStep()
{
    if (g_EglDisplay == EGL_NO_DISPLAY)
        return;

    // Setup display size (every frame to accommodate for window resizing)
    int32_t window_width = ANativeWindow_getWidth(g_App->window);
    int32_t window_height = ANativeWindow_getHeight(g_App->window);

    glViewport(0, 0, (int)window_width, (int)window_height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Use shader program
    glUseProgram(program);

    // Set up vertex data
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set up vertex attributes
    GLint positionAttrib = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(positionAttrib);
    glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);

    GLint texCoordAttrib = glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(texCoordAttrib);
    glVertexAttribPointer(texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(program, "uTexture"), 0);

    // Draw elements
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

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

void android_main(struct android_app* state)
{
    state->onAppCmd = handleAppCmd;

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

        if (g_Initialized) {
            MainLoopStep();
        }
    }
}