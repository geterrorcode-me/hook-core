# BlackHook Core Engine 🚀

**BlackHook Core** adalah modul inti (Library AAR) untuk proyek **virtual manager**. Modul ini berfungsi sebagai *Hooking Engine* yang menggabungkan kekuatan Java Reflection dan Native C++ (NDK) untuk melakukan manipulasi memori dan bypass sistem pada Android.

## 🏗️ Arsitektur Proyek

Engine ini menggunakan arsitektur hybrid:
* **Java Layer**: Mengatur inisialisasi dan komunikasi dengan Android Framework.
* **Native Layer (C++)**: Melakukan patching instruksi dan hooking pada level sistem.
* **Bypass Layer**: Menggunakan `reflection.jar` untuk menembus batasan Hidden API Android.

## 📁 Struktur Folder

* `app/libs/`: Tempat penyimpanan library dependensi (`reflection.jar`).
* `app/src/main/java/`: Source code Java untuk Manager Engine.
* `app/src/main/jni/`: Source code C++ (Native) dan konfigurasi NDK.
* `.github/workflows/`: Otomatisasi build menggunakan GitHub Actions.

## 🚀 Fitur Utama

- [x] **Hidden API Bypass**: Menghapus batasan akses API internal Android.
- [x] **Native Bridge**: Koneksi JNI yang dioptimalkan untuk performa tinggi.
- [x] **Multi-ABI Support**: Mendukung arsitektur `armeabi-v7a` dan `arm64-v8a`.
- [x] **Automated Build**: CI/CD menggunakan GitHub Actions untuk menghasilkan `.aar`.

---
<p align="center">
  🚀 <i>Optimized for high-performance ARM64 architecture.</i>
</p>

<p align="center">
Developed with ❤️ by <b>geterrorcod-me</b>

<i>"Control the framework, control the system."</i>
</p>


