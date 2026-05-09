#include <jni.h>
#include <sys/system_properties.h>
#include <android/log.h>
#include <string.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "dobby.h" // Pastikan Dobby sudah ada di path JNI

#define LOG_TAG "VPhoneOS-VFS"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Lokasi penyimpanan file virtual kita di internal data
const char* REDIRECT_BASE = "/data/user/0/com.aar.test/files/virtual_os";

// Fungsi asli yang akan kita simpan
int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode);
int (*orig_access)(const char *pathname, int mode);

// Logika pembelokan jalur
const char* get_redirected_path(const char* path) {
    if (path == nullptr) return nullptr;

    // 1. Belokkan build.prop
    if (strcmp(path, "/system/build.prop") == 0 || strstr(path, "build.prop")) {
        LOGI("VFS: Redirecting build.prop -> %s/my_build.prop", REDIRECT_BASE);
        static char new_path[256];
        sprintf(new_path, "%s/my_build.prop", REDIRECT_BASE);
        return new_path;
    }

    // 2. Belokkan framework.jar (Inti dari riset kamu)
    if (strstr(path, "/system/framework/framework.jar")) {
        LOGI("VFS: Redirecting framework.jar -> %s/framework.jar", REDIRECT_BASE);
        static char new_path[256];
        sprintf(new_path, "%s/framework.jar", REDIRECT_BASE);
        return new_path;
    }

    return path;
}

// Hook Fungsi openat
int fake_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    return orig_openat(dirfd, get_redirected_path(pathname), flags, mode);
}

// Hook Fungsi access (Agar sistem percaya file virtual itu ada)
int fake_access(const char *pathname, int mode) {
    return orig_access(get_redirected_path(pathname), mode);
}

extern "C" {

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine VFS: Initializing I/O Redirector...");

    void* libc = dlopen("libc.so", RTLD_LAZY);
    if (!libc) return;

    // Hook openat
    void* openat_addr = dlsym(libc, "openat");
    if (openat_addr) {
        DobbyHook(openat_addr, (void *)fake_openat, (void **)&orig_openat);
        LOGI("VFS: openat Hooked!");
    }

    // Hook access
    void* access_addr = dlsym(libc, "access");
    if (access_addr) {
        DobbyHook(access_addr, (void *)fake_access, (void **)&orig_access);
        LOGI("VFS: access Hooked!");
    }
    
    dlclose(libc);
    LOGI("Engine VFS: All Core Hooks Applied.");
}

}
