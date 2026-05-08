#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_TAG "BlackHook-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C"
JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("1.0.0-Hybrid-Engine");
}

extern "C"
JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Mencoba melakukan hooking pada method melalui native layer...");
    // Di sini nantinya kita masukkan logika SandHook atau Zygisk bridge
}
