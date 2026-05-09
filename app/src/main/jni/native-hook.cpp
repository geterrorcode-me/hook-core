#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/system_properties.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"

#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Pointer fungsi asli
void (*orig_prop_read_cb)(const prop_info* pi, void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial), void* cookie);

// Callback spoofing yang sangat hati-hati
void my_property_callback(void* cookie, const char* name, const char* value, uint32_t serial) {
    auto real_cb = (void (*)(void*, const char*, const char*, uint32_t))((void**)cookie)[0];
    void* real_cookie = ((void**)cookie)[1];
    
    if (name != nullptr && value != nullptr) {
        if (strcmp(name, "ro.product.model") == 0) {
            real_cb(real_cookie, name, "SM-S918B", serial);
            return;
        }
        if (strcmp(name, "ro.product.brand") == 0 || strcmp(name, "ro.product.manufacturer") == 0) {
            real_cb(real_cookie, name, "samsung", serial);
            return;
        }
    }

    if (real_cb) {
        real_cb(real_cookie, name, value, serial);
    }
}

void fake_prop_read_cb(const prop_info* pi, void (*callback)(void*, const char*, const char*, uint32_t), void* cookie) {
    if (callback == nullptr || orig_prop_read_cb == nullptr) return;
    void* wrapper[2] = {(void*)callback, cookie};
    orig_prop_read_cb(pi, my_property_callback, wrapper);
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("2.4.0-Ultra-Stable");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine Native: Attempting Hook...");

    // Gunakan RTLD_NOW agar simbol langsung dimuat
    void* libc_handle = dlopen("libc.so", RTLD_NOW);
    if (libc_handle) {
        void* prop_cb_addr = dlsym(libc_handle, "__system_property_read_callback");
        if (prop_cb_addr) {
            // Gunakan DobbyHook dengan verifikasi
            DobbyHook(prop_cb_addr, (void *)fake_prop_read_cb, (void **)&orig_prop_read_cb);
            LOGI("Engine Native: Hook Success");
        }
        dlclose(libc_handle);
    }
}

}
