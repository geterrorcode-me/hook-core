package black.hook;

import android.util.Log;

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

    public static native String getEngineVersion();
    public static native void nativeHookMethod(Object method);

    public static void init() {
        try {
            // Kita gunakan refleksi untuk memanggil HiddenApiBypass 
            // Agar jika library tidak ada, aplikasi tidak langsung mati saat compile
            if (android.os.Build.VERSION.SDK_INT >= 28) { // Android P ke atas
                Class<?> bypassClass = Class.forName("org.lsposed.hiddenapibypass.HiddenApiBypass");
                java.lang.reflect.Method addMethods = bypassClass.getDeclaredMethod("addAllowedExternalMethods", String[].class);
                addMethods.invoke(null, (Object) new String[]{"L"});
                Log.i(TAG, "Bypass Hidden API Success via Reflection");
            }
        } catch (Exception e) {
            Log.e(TAG, "Bypass Hidden API Failed: " + e.getMessage());
        }
        
        try {
            Log.i(TAG, "vPhoneOS Engine Version: " + getEngineVersion());
        } catch (Throwable t) {
            Log.e(TAG, "Native Method getEngineVersion not found!");
        }
    }
}
