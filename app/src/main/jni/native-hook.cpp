#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <sys/system_properties.h>
#include <android/log.h>
#include <dlfcn.h>
#include "dobby.h"

#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// --- Penampung Fungsi Asli ---
int (*orig_open)(const char *pathname, int flags, ...);
int (*orig_prop_get)(const char *name, char *value);
void (*orig_prop_read_cb)(const prop_info* pi, void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial), void* cookie);

// --- 1. Logika Pengalihan File (File Redirection) ---
int fake_open(const char *pathname, int flags, mode_t mode) {
    if (pathname != nullptr && strstr(pathname, "/data/system/")) {
        std::string virtual_path = "/data/data/com.aar.test/virtual_os"; 
        virtual_path += (pathname + 12);
        LOGI("Redirect: %s -> %s", pathname, virtual_path.c_str());
        return orig_open(virtual_path.c_str(), flags, mode);
    }
    return orig_open(pathname, flags, mode);
}

// --- 2. Logika Spoofing Modern (Callback Method) ---
// Ini adalah jantung pertahanan untuk menembus sensor Android 12-15
void my_prop_read_callback(void* cookie, const char* name, const char* value, uint32_t serial) {
    // Ambil callback asli yang disimpan di cookie
    auto real_cb = (void (*)(void*, const char*, const char*, uint32_t))((void**)cookie)[0];
    void* real_cookie = ((void**)cookie)[1];
    
    const char* final_val = value;

    // Intersepsi data di sini
    if (strcmp(name, "ro.product.model") == 0) {
        final_val = "SM-S918B";
    } else if (strcmp(name, "ro.product.brand") == 0) {
        final_val = "samsung";
    } else if (strcmp(name, "ro.product.manufacturer") == 0) {
        final_val = "samsung";
    } else if (strcmp(name, "ro.build.version.release") == 0) {
        final_val = "15";
    }

    // Teruskan ke aplikasi dengan data yang sudah dipalsukan
    real_cb(real_cookie, name, final_val, serial);
}

void fake_prop_read_cb(const prop_info* pi, void (*callback)(void* cookie, const char* name, const char* value, uint32_t serial), void* cookie) {
    // Kita bungkus callback asli ke dalam wrapper kita sendiri
    void* wrapper_data[2] = {(void*)callback, cookie};
    orig_prop_read_cb(pi, my_prop_read_callback, wrapper_data);
}

// --- 3. Logika Spoofing Klasik (Legacy Method) ---
int fake_prop_get(const char *name, char *value) {
    int len = orig_prop_get(name, value);
    if (name == nullptr) return len;

    if (strcmp(name, "ro.product.model") == 0) {
        strcpy(value, "SM-S918B");
        return strlen(value);
    }
    return len;
}

extern "C" {

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Memulai Bypass S23 Ultra pada Device Modern...");

    void* libc_handle = dlopen("libc.so", RTLD_LAZY);
    if (libc_handle) {
        // Hook Open (File)
        void* open_addr = dlsym(libc_handle, "open");
        if (open_addr) DobbyHook(open_addr, (void *)fake_open, (void **)&orig_open);

        // Hook Property Get (Legacy)
        void* prop_get_addr = dlsym(libc_handle, "__system_property_get");
        if (prop_get_addr) DobbyHook(prop_get_addr, (void *)fake_prop_get, (void **)&orig_prop_get);

        // Hook Property Callback (Modern - WAJIB UNTUK ANDROID 12+)
        void* prop_read_cb_addr = dlsym(libc_handle, "__system_property_read_callback");
        if (prop_read_cb_addr) {
            DobbyHook(prop_read_cb_addr, (void *)fake_prop_read_cb, (void **)&orig_prop_read_cb);
            LOGI("System Property Callback Hooked!");
        }

        dlclose(libc_handle);
    }
    LOGI("vPhoneOS Engine Active!");
}

}
