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

// Definisi tipe fungsi untuk dlsym
typedef int (*PFN_PROP_UPDATE)(prop_info*, const char*, unsigned int);
typedef int (*PFN_PROP_ADD)(const char*, unsigned int, const char*, unsigned int);

#define LOG_TAG "VPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* REDIRECT_BASE = "/data/user/0/com.aar.test/files/virtual_os";

// Pointer fungsi asli untuk VFS
int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode);
int (*orig_access)(const char *pathname, int mode);

// Helper: Membelokkan jalur file
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

// Helper: Paksa tulis properti ke memori menggunakan dlsym (Bypass Linker Error)
void patch_system_property(const char* key, const char* value) {
    // Cari alamat fungsi internal libc secara dinamis
    PFN_PROP_UPDATE prop_update = (PFN_PROP_UPDATE) dlsym(RTLD_DEFAULT, "__system_property_update");
    PFN_PROP_ADD prop_add = (PFN_PROP_ADD) dlsym(RTLD_DEFAULT, "__system_property_add");

    const prop_info* pi = __system_property_find(key);
    if (pi && prop_update) {
        prop_update((prop_info*)pi, value, strlen(value));
        LOGI("Identity: Patched %s -> %s", key, value);
    } else if (prop_add) {
        prop_add(key, strlen(key), value, strlen(value));
        LOGI("Identity: Added %s -> %s", key, value);
    } else {
        LOGI("Identity: Gagal patch %s (Symbol not found)", key);
    }
}

// Wrapper Hook Functions
int fake_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    return orig_openat(dirfd, get_redirected_path(pathname), flags, mode);
}

int fake_access(const char *pathname, int mode) {
    return orig_access(get_redirected_path(pathname), mode);
}

extern "C" {

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine VFS: Initializing I/O Redirector...");

    void* openat_addr = dlsym(RTLD_DEFAULT, "openat");
    void* access_addr = dlsym(RTLD_DEFAULT, "access");

    if (openat_addr) DobbyHook(openat_addr, (void *)fake_openat, (void **)&orig_openat);
    if (access_addr) DobbyHook(access_addr, (void *)fake_access, (void **)&orig_access);

    LOGI("Engine Identity: Synchronizing Memory Properties...");
    patch_system_property("ro.product.model", "SM-S918B");
    patch_system_property("ro.product.brand", "samsung");
    patch_system_property("ro.product.manufacturer", "samsung");
    patch_system_property("ro.build.product", "dm3q");
    patch_system_property("ro.product.device", "dm3q");

    LOGI("VPhoneOS Core v4.1: Fully Active.");
}

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("4.1.1-VFS-Identity-Fixed");
}

}
