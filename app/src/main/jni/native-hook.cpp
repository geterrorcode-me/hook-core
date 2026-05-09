#include <jni.h>
#include <sys/system_properties.h>
#include <android/log.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <linux/ioctl.h>
#include "dobby.h"

// Definisi untuk Binder Virtualization
#define BINDER_WRITE_READ _IOWR('b', 1, struct binder_write_read)

#define LOG_TAG "VPhoneOS-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char* REDIRECT_BASE = "/data/user/0/com.aar.test/files/virtual_os";

// Pointer Fungsi Asli
int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode);
int (*orig_access)(const char *pathname, int mode);
int (*orig_get_prop)(const char *name, char *value);
int (*orig_ioctl)(int fd, unsigned long request, void *arg);

// Helper: VFS Redirector
const char* get_redirected_path(const char* path) {
    if (path == nullptr) return nullptr;
    static char new_path[512];
    if (strstr(path, "build.prop")) {
        snprintf(new_path, sizeof(new_path), "%s/my_build.prop", REDIRECT_BASE);
        return new_path;
    }
    return path;
}

// Hook: Identitas Samsung (Anti-FC)
int fake_get_prop(const char *name, char *value) {
    if (strcmp(name, "ro.product.model") == 0) {
        strcpy(value, "SM-S918B");
        return strlen(value);
    }
    if (strcmp(name, "ro.product.brand") == 0) {
        strcpy(value, "samsung");
        return strlen(value);
    }
    return orig_get_prop(name, value);
}

// Hook: I/O Redirector (VFS)
int fake_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    return orig_openat(dirfd, get_redirected_path(pathname), flags, mode);
}

// Hook: Binder Virtualization (Mencegat komunikasi antar proses)
int fake_ioctl(int fd, unsigned long request, void *arg) {
    if (request == BINDER_WRITE_READ) {
        // Di sini nantinya AMS Virtual bekerja memanipulasi Intent
    }
    return orig_ioctl(fd, request, arg);
}

extern "C" {
JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("VPhoneOS: Initializing Engine Core...");

    void* openat_ptr = dlsym(RTLD_DEFAULT, "openat");
    void* getprop_ptr = dlsym(RTLD_DEFAULT, "__system_property_get");
    void* ioctl_ptr = dlsym(RTLD_DEFAULT, "ioctl");

    if (openat_ptr) DobbyHook(openat_ptr, (void *)fake_openat, (void **)&orig_openat);
    if (getprop_ptr) DobbyHook(getprop_ptr, (void *)fake_get_prop, (void **)&orig_get_prop);
    
    // Binder Virtualization Bridge
    if (ioctl_ptr) {
        DobbyHook(ioctl_ptr, (void *)fake_ioctl, (void **)&orig_ioctl);
        LOGI("VPhoneOS: Binder Bridge Active.");
    }

    LOGI("VPhoneOS: Native Engine Stable.");
}

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("5.0.0-AMS-Binder-Alpha");
}
}
