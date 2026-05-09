#include <jni.h>
#include <sys/system_properties.h>
#include <android/log.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "dobby.h"

#define LOG_TAG "VPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* REDIRECT_BASE = "/data/user/0/com.aar.test/files/virtual_os";

// --- Pointer Fungsi Asli ---
int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode);
int (*orig_access)(const char *pathname, int mode);
int (*orig_get_prop)(const char *name, char *value);

// --- Jalur VFS (Virtual File System) ---
const char* get_redirected_path(const char* path) {
    if (path == nullptr) return nullptr;
    static char new_path[512];
    
    if (strstr(path, "build.prop")) {
        snprintf(new_path, sizeof(new_path), "%s/my_build.prop", REDIRECT_BASE);
        return new_path;
    }
    if (strstr(path, "framework.jar") && strstr(path, "/system/")) {
        snprintf(new_path, sizeof(new_path), "%s/framework.jar", REDIRECT_BASE);
        return new_path;
    }
    return path;
}

// --- Fake Function: Identity Spoofing (Anti-FC) ---
// Kita membajak fungsi GET, bukan melakukan PATCH ke memori sistem yang dilindungi
int fake_get_prop(const char *name, char *value) {
    if (strcmp(name, "ro.product.model") == 0) {
        strcpy(value, "SM-S918B");
        return strlen(value);
    }
    if (strcmp(name, "ro.product.brand") == 0 || strcmp(name, "ro.product.manufacturer") == 0) {
        strcpy(value, "samsung");
        return strlen(value);
    }
    if (strcmp(name, "ro.build.product") == 0 || strcmp(name, "ro.product.device") == 0) {
        strcpy(value, "dm3q");
        return strlen(value);
    }
    return orig_get_prop(name, value);
}

// --- Wrapper VFS ---
int fake_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    return orig_openat(dirfd, get_redirected_path(pathname), flags, mode);
}

int fake_access(const char *pathname, int mode) {
    return orig_access(get_redirected_path(pathname), mode);
}

extern "C" {
JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine: Memulai Prosedur Safety-Hooking...");

    void* openat_ptr = dlsym(RTLD_DEFAULT, "openat");
    void* access_ptr = dlsym(RTLD_DEFAULT, "access");
    void* getprop_ptr = dlsym(RTLD_DEFAULT, "__system_property_get");

    // Eksekusi Hooking
    if (openat_ptr) DobbyHook(openat_ptr, (void *)fake_openat, (void **)&orig_openat);
    if (access_ptr) DobbyHook(access_ptr, (void *)fake_access, (void **)&orig_access);
    
    // Safety Layer: Mencegat request identitas (Anti-FC)
    if (getprop_ptr) {
        DobbyHook(getprop_ptr, (void *)fake_get_prop, (void **)&orig_get_prop);
        LOGI("Engine: Identity Hooking Aktif.");
    }

    LOGI("VPhoneOS Core v4.1.2: Semua Saraf Virtual Terpasang.");
}

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("4.1.2-VFS-SafeSync");
}
}
