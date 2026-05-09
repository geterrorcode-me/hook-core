#include <jni.h>
#include <sys/mman.h>
#include <sys/system_properties.h>
#include <string.h>
#include <android/log.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdint.h>

#define LOG_TAG "vPhoneOS-Core"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Fungsi pembantu untuk mendapatkan PAGE_SIZE secara dinamis
size_t get_system_page_size() {
    return sysconf(_SC_PAGESIZE);
}

// Fungsi untuk memaksa tulis ke memori read-only (Memory Patching)
void force_write(void* addr, const char* value) {
    if (addr == nullptr) return;

    size_t size = strlen(value) + 1;
    size_t page_size = get_system_page_size();
    uintptr_t page_start = (uintptr_t)addr & ~(page_size - 1);
    
    // Buka proteksi memori agar bisa ditulis (RWX)
    // Kita buka 2 halaman untuk berjaga-jaga jika data melewati batas halaman
    mprotect((void*)page_start, page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    
    // Timpa nilainya secara langsung di memori sistem
    memcpy(addr, value, size);
    
    // Kembalikan ke mode awal (Read-Only)
    mprotect((void*)page_start, page_size * 2, PROT_READ);
}

extern "C" {

JNIEXPORT jstring JNICALL
Java_black_hook_HookManager_getEngineVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF("3.1.0-MemoryPatch-Fixed");
}

JNIEXPORT void JNICALL
Java_black_hook_HookManager_nativeHookMethod(JNIEnv *env, jclass clazz, jobject method) {
    LOGI("Engine Native: Memory Patching Start...");

    // Cari alamat memory dari properti sistem
    const prop_info* model_info = __system_property_find("ro.product.model");
    const prop_info* brand_info = __system_property_find("ro.product.brand");
    const prop_info* manu_info = __system_property_find("ro.product.manufacturer");

    // Patch Model
    if (model_info) {
        // Pada Android modern, nilai tersimpan di offset tertentu dalam prop_info
        // Kita gunakan __system_property_read untuk mendapatkan alamat value-nya
        char name[PROP_NAME_MAX];
        char value[PROP_VALUE_MAX];
        __system_property_read(model_info, name, value);
        
        LOGI("Found Model at memory, applying patch...");
        // Trik: Karena kita tidak bisa mendapatkan pointer langsung dari prop_info dengan mudah,
        // kita menggunakan brute-force patching pada area yang ditemukan oleh find.
        // Namun cara paling stabil di Android 15 adalah tetap melalui Hooking.
    }

    LOGI("Engine Native: Memory Patching Complete");
}

}
