LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := flappybird
LOCAL_SRC_FILES := android_native_app_glue.c main.c
LOCAL_LDLIBS    := -llog -landroid -lGLESv2 -lEGL -u ANativeActivity_onCreate

include $(BUILD_SHARED_LIBRARY)