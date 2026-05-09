#include <jni.h>
#include <sys/system_properties.h>
#include <android/log.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>  // <--- Tambahan wajib untuk sprintf/snprintf
#include "dobby.h"

#define LOG_TAG "VPhoneOS-VFS"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Lokasi penyimpanan file virtual kita
const char* REDIRECT_BASE = "/data/user/0/com.aar.test/files/virtual_os";

// Penunjuk fungsi asli
int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode);
int (*orig_access)(const char *pathname, int mode);

// Logika pembelokan jalur yang lebih aman
const char* get_redirected_path(const char* path) {
    if (path == nullptr) return nullptr;

    // Buffer statis untuk menampung path baru agar tidak hilang saat fungsi return
    static char new_path[512]; 

    // 1. Belokkan build.prop
    if (strstr(path, "build.prop")) {
        snprintf(new_path, sizeof(new_path), "%s/my_build.prop", REDIRECT_BASE);
        LOGI("VFS: Redirecting -> %s", new_path);
        return new_path;
    }

    // 2. Belokkan framework.jar
    if (strstr(path, "framework.jar") && strstr(path, "/system/")) {
        snprintf(new_path, sizeof(new_path), "%s/framework.jar", REDIRECT_BASE);
        LOGI("VFS: Redirecting -> %s", new_path);
        return new_path;
    }

    return path;
}

// Wrapper Hook openat
int fake_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    const char* redirected = get_redirected_path(pathname);
    return orig_openat(dirfd, redirected, flags, mode);
}

// Wrapper Hook access
int fake_access(const char *pathname, int mode) {
    const char* redirected = get_redirected_path(pathname);
    return orig_access(redirected, mode);
}

extern "C" {

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine VFS: Initializing I/O Redirector...");

    // Gunakan RTLD_DEFAULT untuk mencari symbol di library yang sudah terload
    void* openat_addr = dlsym(RTLD_DEFAULT, "openat");
    void* access_addr = dlsym(RTLD_DEFAULT, "access");

    if (openat_addr) {
        DobbyHook(openat_addr, (void *)fake_openat, (void **)&orig_openat);
        LOGI("VFS: openat Hooked successfully.");
    }

    if (access_addr) {
        DobbyHook(access_addr, (void *)fake_access, (void **)&orig_access);
        LOGI("VFS: access Hooked successfully.");
    }
    
    LOGI("Engine VFS: Upgrade 4.0 Active.");
}

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("4.0.0-VFS-Core");
}

}
