#include <jni.h>
#include <string>
#include <android/log.h>
#include "dobby.h" // SEKARANG SUDAH AMAN

#define LOG_TAG "BlackHook-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    // Memanggil fungsi Dobby untuk mendapatkan versi Dobby itu sendiri
    return env->NewStringUTF("1.0.0-Dobby-Integrated");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Dobby Hook Engine siap digunakan!");
    // Di sinilah kita akan memanggil DobbyHook nantinya
}

}
