#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/system_properties.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"

#define LOG_TAG "vMeer-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int (*orig_open)(const char *pathname, int flags, ...);
void (*orig_prop_read_cb)(const prop_info* pi, void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial), void* cookie);

// --- File Redirector ---
int fake_open(const char *pathname, int flags, mode_t mode) {
    if (pathname != nullptr && strstr(pathname, "/data/system/mcd/")) {
        // Redirect ke folder internal yang kita buat di Java tadi
        std::string p = pathname;
        std::string fname = p.substr(p.find_last_of("/") + 1);
        std::string vpath = "/data/user/0/com.aar.test/files/virtual_os/mcd/" + fname;
        
        LOGI("Redirecting: %s -> %s", pathname, vpath.c_str());
        return orig_open(vpath.c_str(), flags, mode);
    }
    return orig_open(pathname, flags, mode);
}

// --- Property Spoofing (S23 Ultra) ---
void my_callback(void* cookie, const char* name, const char* value, uint32_t serial) {
    auto real_cb = (void (*)(void*, const char*, const char*, uint32_t))((void**)cookie)[0];
    void* real_cookie = ((void**)cookie)[1];
    
    const char* val = value;
    if (strcmp(name, "ro.product.model") == 0) val = "SM-S918B";
    if (strcmp(name, "ro.product.brand") == 0) val = "samsung";
    if (strcmp(name, "ro.product.manufacturer") == 0) val = "samsung";

    real_cb(real_cookie, name, val, serial);
}

void fake_prop_read_cb(const prop_info* pi, void (*callback)(void*, const char*, const char*, uint32_t), void* cookie) {
    void* wrapper[2] = {(void*)callback, cookie};
    orig_prop_read_cb(pi, my_callback, wrapper);
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("2.1.0-S23U-MCD-Fix");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    void* libc = dlopen("libc.so", RTLD_LAZY);
    if (libc) {
        void* op = dlsym(libc, "open");
        if (op) DobbyHook(op, (void *)fake_open, (void **)&orig_open);

        void* cb = dlsym(libc, "__system_property_read_callback");
        if (cb) DobbyHook(cb, (void *)fake_prop_read_cb, (void **)&orig_prop_read_cb);
        
        dlclose(libc);
    }
}

}
