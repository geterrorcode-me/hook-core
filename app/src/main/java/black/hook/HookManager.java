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

    public static native String getEngineVersion();
    public static native void nativeHookMethod(Member method);

    public static void init() {
        // Panggil method bypass() sesuai hasil javap tadi
        try {
            boolean success = HiddenApiBypass.bypass();
            Log.i(TAG, "Bypass Hidden API status: " + success);
        } catch (Throwable e) {
            Log.e(TAG, "Gagal menjalankan bypass: " + e.getMessage());
        }
        
        Log.i(TAG, "BlackHook Engine Version: " + getEngineVersion());
    }
}
