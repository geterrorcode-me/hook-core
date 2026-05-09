#include <jni.h>
#include <unistd.h>
#include <string.h>
#include <sys/system_properties.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"

#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// --- Pointer Fungsi Asli ---
int (*orig_get)(const char* name, char* value);
void (*orig_read_cb)(const prop_info* pi, void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial), void* cookie);

// --- LOGIKA SPOOFING ---
const char* spoof_property(const char* name, const char* original_value) {
    if (name == nullptr) return original_value;
    
    if (strcmp(name, "ro.product.model") == 0 || strcmp(name, "ro.product.system.model") == 0) {
        return "SM-S918B";
    }
    if (strcmp(name, "ro.product.brand") == 0 || strcmp(name, "ro.product.manufacturer") == 0) {
        return "samsung";
    }
    return original_value;
}

// --- FAKE FUNCTIONS ---

// 1. Hook untuk __system_property_get
int fake_get(const char* name, char* value) {
    int len = orig_get(name, value);
    const char* spoofed = spoof_property(name, value);
    if (spoofed != value) {
        strcpy(value, spoofed);
        return strlen(value);
    }
    return len;
}

// 2. Hook untuk __system_property_read_callback
void my_property_callback(void* cookie, const char* name, const char* value, uint32_t serial) {
    auto real_cb = (void (*)(void*, const char*, const char*, uint32_t))((void**)cookie)[0];
    void* real_cookie = ((void**)cookie)[1];
    real_cb(real_cookie, name, spoof_property(name, value), serial);
}

void fake_read_cb(const prop_info* pi, void (*callback)(void*, const char*, const char*, uint32_t), void* cookie) {
    void* wrapper[2] = {(void*)callback, cookie};
    orig_read_cb(pi, my_property_callback, wrapper);
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("2.5.0-MultiHook-S23U");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine Native: Starting Multi-Hook...");

    void* libc = dlopen("libc.so", RTLD_NOW);
    if (libc) {
        // Hook Metode 1 (Callback)
        void* read_cb_addr = dlsym(libc, "__system_property_read_callback");
        if (read_cb_addr) DobbyHook(read_cb_addr, (void *)fake_read_cb, (void **)&orig_read_cb);

        // Hook Metode 2 (Legacy Get) - Sering dipakai aplikasi lama/game
        void* get_addr = dlsym(libc, "__system_property_get");
        if (get_addr) DobbyHook(get_addr, (void *)fake_get, (void **)&orig_get);

        LOGI("Engine Native: Multi-Hook Applied!");
        dlclose(libc);
    }
}

}
