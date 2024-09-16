LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := flappybird
LOCAL_SRC_FILES := android_native_app_glue.c main.c upng.c
LOCAL_LDLIBS    := -llog -landroid -lGLESv2 -lEGL -lOpenSLES -u ANativeActivity_onCreate
LOCAL_CFLAGS    := -Os -fvisibility=hidden -ffunction-sections -fdata-sections

include $(BUILD_SHARED_LIBRARY)