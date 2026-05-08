package black.hook;

import black.reflection.HiddenApiBypass;
import java.lang.reflect.Member;
import android.util.Log;

public class HookManager {
    private static final String TAG = "BlackHook";

    static {
        try {
            System.loadLibrary("blackhook");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Gagal memuat library native: " + e.getMessage());
        }
    }

    // Fungsi Native
    public static native String getEngineVersion();
    public static native void nativeHookMethod(Member method);

    public static void init() {
        // Melakukan bypass Hidden API agar engine bisa bekerja
        boolean success = HiddenApiBypass.addRuntimeWhiteList("L;");
        
        Log.i(TAG, "Bypass Hidden API status: " + success);
        Log.i(TAG, "BlackHook Engine Version: " + getEngineVersion());
    }
}
