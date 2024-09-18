LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := flappybird
LOCAL_LDLIBS    := -llog -landroid -lGLESv2 -lEGL -lOpenSLES -lm -u ANativeActivity_onCreate

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c)
#FILE_LIST += $(wildcard $(LOCAL_PATH)/folder/*.c)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_CFLAGS    := -Oz -fvisibility=hidden -ffunction-sections -fdata-sections -fno-stack-protector -fomit-frame-pointer -flto
LOCAL_LDFLAGS   := -Wl,--gc-sections -s

include $(BUILD_SHARED_LIBRARY)