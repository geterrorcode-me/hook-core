package black.hook;

import android.util.Log;
import java.lang.reflect.Method;

public class HookManager {
    private static final String TAG = "BlackHook-Java";

    static {
        try {
            System.loadLibrary("blackhook");
        } catch (Exception e) {
            Log.e(TAG, "Library Load Fail");
        }
    }

    public static native String getEngineVersion();
    public static native void nativeHookMethod(Object method);

    public static void init() {
        // Bypass Hidden API di Java (Lebih aman dari crash native)
        try {
            Class<?> vmRuntimeClass = Class.forName("dalvik.system.VMRuntime");
            Method getRuntimeMethod = vmRuntimeClass.getDeclaredMethod("getRuntime");
            Object runtime = getRuntimeMethod.invoke(null);
            Method setExemptions = vmRuntimeClass.getDeclaredMethod("setHiddenApiExemptions", String[].class);
            setExemptions.invoke(runtime, (Object) new String[]{"L"});
            Log.i(TAG, "Java Bypass OK");
        } catch (Throwable t) {
            Log.e(TAG, "Java Bypass Error: " + t.getMessage());
        }

        // Panggil native
        try {
            nativeHookMethod(null);
        } catch (Throwable t) {
            Log.e(TAG, "Native Hook Error");
        }
    }
}
