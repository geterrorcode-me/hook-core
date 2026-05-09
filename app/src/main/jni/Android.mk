LOCAL_PATH := $(call my-dir)

# --- Konfigurasi Dobby Engine ---
include $(CLEAR_VARS)
LOCAL_MODULE := dobby
LOCAL_SRC_FILES := libdobby.a # Jika kamu pakai static library
include $(PREBUILT_STATIC_LIBRARY)

# --- Konfigurasi BlackHook ---
include $(CLEAR_VARS)
LOCAL_MODULE    := blackhook
LOCAL_SRC_FILES := native-hook.cpp
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := dobby
LOCAL_LDLIBS    := -llog

include $(BUILD_SHARED_LIBRARY)
