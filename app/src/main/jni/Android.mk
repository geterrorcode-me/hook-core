LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := blackhook
LOCAL_SRC_FILES := native-hook.cpp
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
