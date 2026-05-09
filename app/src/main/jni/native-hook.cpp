#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include "dobby.h"

#define LOG_TAG "BlackHook-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// 1. Variabel untuk menyimpan alamat fungsi asli (agar aplikasi tidak crash)
int (*orig_getpid)();

// 2. Fungsi "Palsu" kita
int fake_getpid() {
    LOGI("Diterjang BlackHook: Seseorang memanggil getpid(), kita berikan nilai palsu!");
    return 8888; 
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("1.0.0-Dobby-Final-Active");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Memasang Inline Hook pada getpid...");

    // 3. Eksekusi Hook menggunakan Dobby
    // Parameter: (Alamat Fungsi Target, Fungsi Pengganti, Simpan Fungsi Asli)
    DobbyHook((void *)getpid, (void *)fake_getpid, (void **)&orig_getpid);

    LOGI("Hook Berhasil Pasang! Sistem sekarang akan menganggap PID adalah 8888");
}

}
