#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/system_properties.h>
#include <android/log.h>
#include <dlfcn.h> // WAJIB TAMBAH INI
#include "dobby.h"

#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// --- Penampung Fungsi Asli ---
// Kita gunakan format (int, ...) karena open adalah fungsi variadic
int (*orig_open)(const char *pathname, int flags, ...);
int (*orig_prop_get)(const char *name, char *value);

// --- Logika Pengalihan File ---
int fake_open(const char *pathname, int flags, mode_t mode) {
    if (pathname != nullptr && strstr(pathname, "/data/system/")) {
        std::string virtual_path = "/data/data/black.hook/virtual_os"; 
        virtual_path += (pathname + 12);
        
        LOGI("File Redirected: %s -> %s", pathname, virtual_path.c_str());
        return orig_open(virtual_path.c_str(), flags, mode);
    }
    return orig_open(pathname, flags, mode);
}

// --- Logika Spoofing Identitas HP ---
int fake_prop_get(const char *name, char *value) {
    int len = orig_prop_get(name, value);
    if (name == nullptr) return len;

    if (strcmp(name, "ro.product.model") == 0) {
        strcpy(value, "SM-S918B");
        return strlen(value);
    }
    if (strcmp(name, "ro.product.brand") == 0) {
        strcpy(value, "samsung"); // Brand biasanya huruf kecil (e.g., samsung)
        return strlen(value);
    }
    if (strcmp(name, "ro.build.version.release") == 0) {
        strcpy(value, "15");
        return strlen(value);
    }

    return len;
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("2.0.1-S23Ultra-Fix");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Memulai Inisialisasi Galaxy S23 Ultra Engine...");

    // SOLUSI: Cari alamat fungsi 'open' secara manual dari libc.so
    void* libc_handle = dlopen("libc.so", RTLD_LAZY);
    if (libc_handle) {
        void* open_addr = dlsym(libc_handle, "open");
        if (open_addr) {
            DobbyHook(open_addr, (void *)fake_open, (void **)&orig_open);
            LOGI("Hook 'open' berhasil dipasang via dlsym.");
        }
        
        void* prop_get_addr = dlsym(libc_handle, "__system_property_get");
        if (prop_get_addr) {
            DobbyHook(prop_get_addr, (void *)fake_prop_get, (void **)&orig_prop_get);
            LOGI("Hook '__system_property_get' berhasil dipasang.");
        }
        dlclose(libc_handle);
    }

    LOGI("Galaxy S23 Ultra Engine Berhasil Aktif!");
}

}
