#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/system_properties.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"

// Log tag agar mudah dicari di Logcat
#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// --- Fungsi Asli (Pointer) ---
int (*orig_open)(const char *pathname, int flags, ...);
void (*orig_prop_read_cb)(const prop_info* pi, void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial), void* cookie);

// --- 1. File Redirector (Untuk Xiaomi/Poco Config) ---
int fake_open(const char *pathname, int flags, mode_t mode) {
    if (pathname != nullptr && strstr(pathname, "/data/system/mcd/")) {
        std::string p = pathname;
        std::string fname = p.substr(p.find_last_of("/") + 1);
        // Diarahkan ke folder internal aplikasi yang dibuat di Java
        std::string vpath = "/data/user/0/com.aar.test/files/virtual_os/mcd/" + fname;
        
        LOGI("Redirecting File: %s -> %s", pathname, vpath.c_str());
        return orig_open(vpath.c_str(), flags, mode);
    }
    return orig_open(pathname, flags, mode);
}

// --- 2. Property Spoofing (Mengubah Identitas Device) ---
void my_property_callback(void* cookie, const char* name, const char* value, uint32_t serial) {
    auto real_cb = (void (*)(void*, const char*, const char*, uint32_t))((void**)cookie)[0];
    void* real_cookie = ((void**)cookie)[1];
    
    const char* final_value = value;

    // Logika penggantian nama model dan brand
    if (strcmp(name, "ro.product.model") == 0) {
        final_value = "SM-S918B"; // S23 Ultra
    } else if (strcmp(name, "ro.product.brand") == 0 || strcmp(name, "ro.product.manufacturer") == 0) {
        final_value = "samsung";
    } else if (strcmp(name, "ro.build.version.release") == 0) {
        final_value = "15";
    }

    real_cb(real_cookie, name, final_value, serial);
}

void fake_prop_read_cb(const prop_info* pi, void (*callback)(void*, const char*, const char*, uint32_t), void* cookie) {
    void* wrapper[2] = {(void*)callback, cookie};
    orig_prop_read_cb(pi, my_property_callback, wrapper);
}

// --- JNI EXPORT ---
extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("2.2.0-Final-Android15-Bypass");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine Native Starting...");

    // --- A. NATIVE HIDDEN API BYPASS ---
    // Melakukan bypass langsung dari C++ untuk menembus proteksi Android 15
    jclass vm_runtime_class = env->FindClass("dalvik/system/VMRuntime");
    if (vm_runtime_class) {
        jmethodID get_runtime_fn = env->GetStaticMethodID(vm_runtime_class, "getRuntime", "()Ldalvik/system/VMRuntime;");
        jobject runtime_obj = env->CallStaticObjectMethod(vm_runtime_class, get_runtime_fn);
        jmethodID set_exemptions_fn = env->GetMethodID(vm_runtime_class, "setHiddenApiExemptions", "([Ljava/lang/String;)V");
        
        jobjectArray packages = env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF("L"));
        env->CallVoidMethod(runtime_obj, set_exemptions_fn, packages);
        LOGI("Native Hidden API Bypass: SUCCESS");
    } else {
        LOGE("Native Hidden API Bypass: FAILED (Class not found)");
    }

    // --- B. DOBBY HOOKING ---
    void* libc_handle = dlopen("libc.so", RTLD_LAZY);
    if (libc_handle) {
        // Hook fungsi open() untuk redirect file
        void* open_addr = dlsym(libc_handle, "open");
        if (open_addr) {
            DobbyHook(open_addr, (void *)fake_open, (void **)&orig_open);
            LOGI("Hook open(): OK");
        }

        // Hook system_property untuk spoofing device
        void* prop_cb_addr = dlsym(libc_handle, "__system_property_read_callback");
        if (prop_cb_addr) {
            DobbyHook(prop_cb_addr, (void *)fake_prop_read_cb, (void **)&orig_prop_read_cb);
            LOGI("Hook Property Callback: OK");
        }
        
        dlclose(libc_handle);
    } else {
        LOGE("Failed to load libc.so");
    }
}

}
