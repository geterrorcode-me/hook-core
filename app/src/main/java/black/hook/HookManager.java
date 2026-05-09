package black.hook;

import android.util.Log;
import java.lang.reflect.Method;

public class HookManager {
    private static final String TAG = "BlackHook-Java";

    static {
        System.loadLibrary("blackhook");
    }

    public static native String getEngineVersion();
    public static native void nativeHookMethod(Object method);

    public static void init() {
        try {
            // Cara Bypass Android 14/15 yang lebih stabil
            Class<?> vmRuntimeClass = Class.forName("dalvik.system.VMRuntime");
            Method getRuntimeMethod = vmRuntimeClass.getDeclaredMethod("getRuntime");
            Object runtime = getRuntimeMethod.invoke(null);
            Method setHiddenApiExemptionsMethod = vmRuntimeClass.getDeclaredMethod("setHiddenApiExemptions", String[].class);
            
            // Berikan izin ke semua L (Library/Class)
            setHiddenApiExemptionsMethod.invoke(runtime, (Object) new String[]{"L"});
            Log.i(TAG, "Hidden API Bypass Success (VMRuntime Method)");
        } catch (Exception e) {
            Log.e(TAG, "VMRuntime Bypass Failed, trying alternative...");
            // Alternative: Jika masih gagal, baru gunakan library lsposed
            try {
                 Class<?> lsposed = Class.forName("org.lsposed.hiddenapibypass.HiddenApiBypass");
                 Method m = lsposed.getDeclaredMethod("addAllowedExternalMethods", String[].class);
                 m.invoke(null, (Object) new String[]{"L"});
            } catch (Exception ex) {
                Log.e(TAG, "All Bypass Methods Failed!");
            }
        }

        Log.i(TAG, "Engine Version: " + getEngineVersion());
    }
}
