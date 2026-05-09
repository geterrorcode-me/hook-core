package black.hook;

import android.util.Log;
import java.lang.reflect.Method;

public class HookManager {
    private static final String TAG = "BlackHook-Java";

    static {
        try {
            System.loadLibrary("blackhook");
            Log.i(TAG, "Library native 'blackhook' berhasil dimuat.");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Gagal memuat library native: " + e.getMessage());
        }
    }

    // Deklarasi fungsi native
    public static native String getEngineVersion();
    public static native void nativeHookMethod(Object method);

    public static void init() {
        Log.i(TAG, "Memulai Mega-Bypass Hidden API...");
        
        try {
            // Menggunakan Double Reflection untuk mengelabui restrictor Android 15
            Method forName = Class.class.getDeclaredMethod("forName", String.class);
            Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);

            Class<?> vmRuntimeClass = (Class<?>) forName.invoke(null, "dalvik.system.VMRuntime");
            Method getRuntimeMethod = (Method) getDeclaredMethod.invoke(vmRuntimeClass, "getRuntime", (Class[]) null);
            Object runtime = getRuntimeMethod.invoke(null);
            
            Method setExemptions = (Method) getDeclaredMethod.invoke(vmRuntimeClass, "setHiddenApiExemptions", new Class[]{String[].class});
            setExemptions.invoke(runtime, (Object) new String[]{"L"});
            
            Log.i(TAG, "Java Mega-Bypass Berhasil!");
        } catch (Throwable t) {
            Log.e(TAG, "Mega-Bypass Gagal, mencoba lanjut ke Native: " + t.getMessage());
        }

        // Panggil proses hooking di layer C++
        try {
            nativeHookMethod(null);
            Log.i(TAG, "Engine Version: " + getEngineVersion());
        } catch (Throwable t) {
            Log.e(TAG, "Gagal memanggil nativeHookMethod: " + t.getMessage());
        }
    }
}
