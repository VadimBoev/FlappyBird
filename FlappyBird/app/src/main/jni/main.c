#include <EGL/egl.h>
#include <GLES/gl.h>
#include "android_native_app_glue.h"
#include <android/log.h>
#include <jni.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "flappy", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "flappy", __VA_ARGS__))

struct android_app* gapp;

#include <math.h>

//load png
#include "upng.h"

//audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

//asset manager
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


// Создание и инициализация объекта OpenSL ES
SLObjectItf engineObject = NULL;
SLEngineItf engineEngine;
SLObjectItf outputMixObject = NULL;
SLObjectItf playerObject = NULL;
SLPlayItf playerPlay;

void createEngine() {
    SLresult result;

    // Создание объекта движка
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to create engine");
        return;
    }

    // Инициализация объекта движка
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to realize engine");
        return;
    }

    // Получение интерфейса движка
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to get engine interface");
        return;
    }

    // Создание объекта вывода
    const SLInterfaceID ids[1] = { SL_IID_ENVIRONMENTALREVERB };
    const SLboolean req[1] = { SL_BOOLEAN_FALSE };
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to create output mix");
        return;
    }

    // Инициализация объекта вывода
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to realize output mix");
        return;
    }
}

void createAudioPlayer(AAssetManager* assetManager, const char* assetPath) {
    SLresult result;

    // Открытие файла из assets
    AAsset* audioAsset = AAssetManager_open(assetManager, assetPath, AASSET_MODE_BUFFER);
    if (!audioAsset) {
        LOGE("Failed to open asset file");
        return;
    }

    // Получение буфера и длины файла
    off_t start, length;
    int fd = AAsset_openFileDescriptor(audioAsset, &start, &length);
    AAsset_close(audioAsset);

    if (fd < 0) {
        LOGE("Failed to get file descriptor");
        return;
    }

    // Создание источника данных
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
    SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_MP3 };
    SLDataSource audioSrc = { &loc_fd, &format_mime };

    // Настройка приемника данных
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
    SLDataSink audioSnk = { &loc_outmix, NULL };

    // Создание проигрывателя
    const SLInterfaceID ids[3] = { SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME };
    const SLboolean req[3] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audioSrc, &audioSnk, 3, ids, req);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to create audio player");
        return;
    }

    // Инициализация проигрывателя
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to realize player");
        return;
    }

    // Получение интерфейса воспроизведения
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to get play interface");
        return;
    }

    // Начало воспроизведения
    result = (*playerPlay)->SetPlayState(playerPlay, SL_PLAYSTATE_PLAYING);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("Failed to start playing");
        return;
    }
}


struct engine {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    float time;
};

static void engine_init_display(struct engine* engine) {
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };

    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    eglChooseConfig(display, attribs, &config, 1, &numConfigs);

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGE("Unable to eglMakeCurrent");
        return;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;

    glViewport(0, 0, w, h);
}

static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        return;
    }

    float red = (sin(engine->time) + 1.0f) / 2.0f;
    float green = (sin(engine->time + 2.0f) + 1.0f) / 2.0f;
    float blue = (sin(engine->time + 4.0f) + 1.0f) / 2.0f;

    glClearColor(red, green, blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(engine->display, engine->surface);
	
	LOGI("test");

    engine->time += 0.01f;
}

static void engine_term_display(struct engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            engine_draw_frame(engine);
            break;
        case APP_CMD_START:
        {

            break;
        }
    }
}

void android_main(struct android_app* state) 
{
    struct engine engine;

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    engine.app = state;

    gapp = state;

    engine.time = 0.0f;

    unsigned char* buffer, *pixels;
    int width_logo, height_logo;
    unsigned long len_file = 0;

    AAsset* file = AAssetManager_open(state->activity->assetManager, "buttons/pause.png", AASSET_MODE_BUFFER);
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

        upng_free(png);
    }

    createEngine();
    createAudioPlayer(state->activity->assetManager, "audio/point.mp3");


    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) {
                source->process(state, source);
            }

            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        engine_draw_frame(&engine);
    }
}