public static void init() {
    try {
        // Coba bypass menggunakan sistem double-reflection
        Method forName = Class.class.getDeclaredMethod("forName", String.class);
        Method getDeclaredMethod = Class.class.getDeclaredMethod("getDeclaredMethod", String.class, Class[].class);

        Class<?> vmRuntimeClass = (Class<?>) forName.invoke(null, "dalvik.system.VMRuntime");
        Method getRuntimeMethod = (Method) getDeclaredMethod.invoke(vmRuntimeClass, "getRuntime", (Class[]) null);
        Object runtime = getRuntimeMethod.invoke(null);
        
        Method setExemptions = (Method) getDeclaredMethod.invoke(vmRuntimeClass, "setHiddenApiExemptions", new Class[]{String[].class});
        setExemptions.invoke(runtime, (Object) new String[]{"L"});
        Log.i(TAG, "Java Mega-Bypass OK");
    } catch (Throwable t) {
        Log.e(TAG, "Bypass Failed, continuing to native...");
    }

    try {
        nativeHookMethod(null);
    } catch (Throwable t) {
        Log.e(TAG, "Native Hook Error");
    }
}
