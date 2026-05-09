#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/system_properties.h>
#include <android/log.h>
#include "dobby.h"

#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// --- Penampung Fungsi Asli ---
int (*orig_open)(const char *pathname, int flags, mode_t mode);
int (*orig_prop_get)(const char *name, char *value);

// --- Logika Pengalihan File ---
int fake_open(const char *pathname, int flags, mode_t mode) {
    // Contoh: Alirkan akses folder sistem ke folder virtual vphoneos
    if (pathname != nullptr && strstr(pathname, "/data/system/")) {
        std::string virtual_path = "/data/data/black.hook/virtual_os"; 
        virtual_path += (pathname + 12); // Lewati "/data/system"
        
        LOGI("File Redirected: %s -> %s", pathname, virtual_path.c_str());
        return orig_open(virtual_path.c_str(), flags, mode);
    }
    return orig_open(pathname, flags, mode);
}

// --- Logika Spoofing Identitas HP ---
int fake_prop_get(const char *name, char *value) {
    int len = orig_prop_get(name, value);
    if (name == nullptr) return len;

    // Ubah Model HP
    if (strcmp(name, "ro.product.model") == 0) {
        strcpy(value, "SM-S918B");
        return strlen(value);
    }
    // Ubah Brand
    if (strcmp(name, "ro.product.brand") == 0) {
        strcpy(value, "Galaxy S23 Ultra");
        return strlen(value);
    }
    // Ubah Versi Android (Contoh: Jadi Android 15)
    if (strcmp(name, "ro.build.version.release") == 0) {
        strcpy(value, "15");
        return strlen(value);
    }

    return len;
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("2.0.0-Full-Virtual-Engine");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Memulai Inisialisasi Galaxy S23 Ultra Engine...");

    // 1. Pasang Hook untuk Pengalihan File
    DobbyHook((void *)open, (void *)fake_open, (void **)&orig_open);

    // 2. Pasang Hook untuk Identitas HP (System Property)
    // Menggunakan __system_property_get dari libc.so
    DobbyHook((void *)__system_property_get, (void *)fake_prop_get, (void **)&orig_prop_get);

    LOGI("Galaxy S23 Ultra Engine Berhasil Aktif!");
}

}
