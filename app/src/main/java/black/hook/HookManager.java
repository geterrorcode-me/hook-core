package black.hook;

import android.util.Log;

public class HookManager {
    static {
        System.loadLibrary("blackhook");
    }

    public static native String getEngineVersion();
    public static native void nativeHookMethod(Object method);

    public static void init() {
        Log.i("BlackHook-Java", "Inisialisasi Engine...");
        // Langsung panggil native, biarkan C++ yang melakukan bypass
        nativeHookMethod(null);
        Log.i("BlackHook-Java", "Version: " + getEngineVersion());
    }
}
