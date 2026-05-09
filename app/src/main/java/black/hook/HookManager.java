package black.hook;

import android.util.Log;
import org.lsposed.hiddenapibypass.HiddenApiBypass;

public class HookManager {
    private static final String TAG = "BlackHook-Java";

    static {
        try {
            // Memuat library native (nama harus sesuai dengan di CMakeLists.txt)
            System.loadLibrary("blackhook");
            Log.i(TAG, "Library native 'blackhook' berhasil dimuat.");
        } catch (UnsatisfiedLinkError e) {
            Log.e(TAG, "Gagal memuat library native: " + e.getMessage());
        }
    }

    // Fungsi Native (Pintu menuju C++)
    public static native String getEngineVersion();
    public static native void nativeHookMethod(Object method);

    /**
     * Inisialisasi Engine
     */
    public static void init() {
        try {
            // Bypass Hidden API (Penting untuk Android 11 ke atas)
            if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.P) {
                // Gunakan cara yang lebih aman untuk Android modern
                boolean success = HiddenApiBypass.addAllowedExternalMethods("L");
                Log.i(TAG, "Bypass Hidden API status: " + success);
            }
            
            // Log versi engine dari native untuk memastikan koneksi OK
            Log.i(TAG, "vPhoneOS Engine Version: " + getEngineVersion());
            
        } catch (Throwable e) {
            // Jika bypass gagal, jangan biarkan aplikasi FC
            Log.e(TAG, "Inisialisasi HookManager bermasalah: " + e.getMessage());
        }
    }
}
