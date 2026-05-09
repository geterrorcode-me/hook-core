LOCAL_PATH := $(call my-dir)

# 1. Deklarasikan Dobby sebagai Prebuilt Library
include $(CLEAR_VARS)
LOCAL_MODULE := dobby_static
LOCAL_SRC_FILES := libs/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)

# 2. Build Library Utama (blackhook)
include $(CLEAR_VARS)
LOCAL_MODULE    := blackhook
LOCAL_SRC_FILES := native-hook.cpp

# Beritahu lokasi header dobby.h
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

# Hubungkan dengan Dobby
LOCAL_STATIC_LIBRARIES := dobby_static
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
