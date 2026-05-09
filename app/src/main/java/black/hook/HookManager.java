package black.hook;

import android.util.Log;

public class HookManager {
    private static final String TAG = "BlackHook-Java";

    static {
        try {
            System.loadLibrary("blackhook");
            Log.i(TAG, "Library native 'blackhook' loaded.");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Fatal: Could not load native library!");
        }
    }

    // Fungsi native
    public static native String getEngineVersion();
    public static native void nativeHookMethod(Object method);

    public static void init() {
        try {
            Log.i(TAG, "Inisialisasi Engine...");
            
            // Panggil hook native
            nativeHookMethod(null);
            
            // Verifikasi versi untuk memastikan bridge hidup
            String version = getEngineVersion();
            Log.i(TAG, "Engine Running Version: " + version);
            
        } catch (Throwable t) {
            Log.e(TAG, "Error saat inisialisasi: " + t.getMessage());
        }
    }
}
