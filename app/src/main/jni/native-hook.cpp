#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/system_properties.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"

#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// Pointer fungsi asli
void (*orig_prop_read_cb)(const prop_info* pi, void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial), void* cookie);

// Callback untuk mengganti identitas device
void my_property_callback(void* cookie, const char* name, const char* value, uint32_t serial) {
    auto real_cb = (void (*)(void*, const char*, const char*, uint32_t))((void**)cookie)[0];
    void* real_cookie = ((void**)cookie)[1];
    
    const char* final_value = value;

    if (name != nullptr) {
        if (strcmp(name, "ro.product.model") == 0) {
            final_value = "SM-S918B"; // S23 Ultra
        } else if (strcmp(name, "ro.product.brand") == 0 || strcmp(name, "ro.product.manufacturer") == 0) {
            final_value = "samsung";
        }
    }

    if (real_cb != nullptr) {
        real_cb(real_cookie, name, final_value, serial);
    }
}

// Handler Hook Property
void fake_prop_read_cb(const prop_info* pi, void (*callback)(void*, const char*, const char*, uint32_t), void* cookie) {
    if (callback == nullptr) {
        orig_prop_read_cb(pi, callback, cookie);
        return;
    }
    void* wrapper[2] = {(void*)callback, cookie};
    orig_prop_read_cb(pi, my_property_callback, wrapper);
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("2.3.0-Stable-S23U");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine Native: Starting Safe Hook Mode...");

    // --- STRATEGI 1: NATIVE BYPASS HIDDEN API ---
    jclass vm_runtime_class = env->FindClass("dalvik/system/VMRuntime");
    if (vm_runtime_class) {
        jmethodID get_runtime_fn = env->GetStaticMethodID(vm_runtime_class, "getRuntime", "()Ldalvik/system/VMRuntime;");
        jobject runtime_obj = env->CallStaticObjectMethod(vm_runtime_class, get_runtime_fn);
        if (runtime_obj) {
            jmethodID set_exemptions_fn = env->GetMethodID(vm_runtime_class, "setHiddenApiExemptions", "([Ljava/lang/String;)V");
            jobjectArray packages = env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF("L"));
            env->CallVoidMethod(runtime_obj, set_exemptions_fn, packages);
            LOGI("Native Bypass: OK");
        }
    }

    // --- STRATEGI 2: DOBBY HOOKING ---
    void* libc_handle = dlopen("libc.so", RTLD_LAZY);
    if (libc_handle) {
        void* prop_cb_addr = dlsym(libc_handle, "__system_property_read_callback");
        if (prop_cb_addr) {
            // Hook hanya pada property untuk menghindari FC saat booting app
            DobbyHook(prop_cb_addr, (void *)fake_prop_read_cb, (void **)&orig_prop_read_cb);
            LOGI("Hook Property Read: SUCCESS");
        }
        dlclose(libc_handle);
    }
    
    LOGI("Engine Native: Hooks Applied Successfully");
}

}
