#include <jni.h>
#include <unistd.h>
#include <android/log.h>
#include "dobby.h"

#define LOG_TAG "BlackHook-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// 1. Siapkan variabel untuk menyimpan alamat fungsi asli
int (*orig_getpid)();

// 2. Buat fungsi "Palsu" (Hook Function)
int fake_getpid() {
    LOGI("Aksi Terdeteksi: Fungsi getpid() dipanggil, tapi BlackHook membelokkannya!");
    // Kita kembalikan nilai palsu, misalnya 8888
    return 8888; 
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("1.0.0-Dobby-Final-Active");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Mencoba memasang Inline Hook pada getpid...");

    // 3. Eksekusi Hook menggunakan Dobby
    // Kita ambil alamat fungsi getpid asli, belokkan ke fake_getpid, simpan aslinya di orig_getpid
    DobbyHook((void *)getpid, (void *)fake_getpid, (void **)&orig_getpid);

    LOGI("Hook Berhasil! Sekarang getpid() akan selalu mengembalikan 8888");
}

}
