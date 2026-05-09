#include <jni.h>
#include <string>
#include <android/log.h>

// Definisi agar kita bisa melihat log di Logcat (Android Studio/Monitor)
#define LOG_TAG "BlackHook-Native"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

/**
 * Fungsi untuk mengecek versi engine dan arsitektur CPU.
 * Dipanggil dari Java: HookManager.getEngineVersion()
 */
JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    std::string version = "1.0.0";
    
#if defined(__arm__)
    version += "-ARM32 (Legacy)";
#elif defined(__aarch64__)
    version += "-ARM64 (Modern)";
#elif defined(__i386__) || defined(__x86_64__)
    version += "-x86/Emulator";
#else
    version += "-Unknown-Arch";
#endif

    LOGI("Engine Version Request: %s", version.c_str());
    return env->NewStringUTF(version.c_str());
}

/**
 * Fungsi inti untuk melakukan hooking.
 * Untuk sekarang, fungsi ini hanya mencatat log bahwa bridge JNI sudah aktif.
 */
JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    // Nantinya di sini kita akan memanggil DobbyHook(...)
    // Saat ini, kita validasi dulu koneksi JNI-nya.
    
    if (method == nullptr) {
        LOGE("Gagal melakukan hook: Method yang dikirim NULL!");
        return;
    }

    LOGI("JNI Bridge Aktif: Siap menerima instruksi hooking untuk method.");
}

} // extern "C"
