![Build Status](https://img.shields.io/github/actions/workflow/status/geterrorcode-me/hook-core/android.yml?style=for-the-badge&logo=github&label=Build)
![Platform](https://img.shields.io/badge/Platform-Android%2010--14+-green?style=for-the-badge&logo=android)
![Architecture](https://img.shields.io/badge/Arch-ARM64--v8a%20%7C%20armeabi--v7a-blue?style=for-the-badge)
![License](https://img.shields.io/badge/License-GPL--3.0-red?style=for-the-badge)


# 🚀 BlackHook Core Engine (AAR)

**BlackHook Core** adalah *high-performance virtualization & hooking engine* yang dirancang khusus untuk ekosistem Android modern. Engine ini menggabungkan teknik **Java Reflection Hooking** dengan **Native Memory Patching (NDK)** untuk menciptakan lingkungan sandbox yang mampu memanipulasi Android Framework dari dalam.

---

## 🏗️ Arsitektur Hybrid (Deep Dive)

BlackHook tidak hanya bekerja di permukaan, ia beroperasi pada tiga lapisan utama:

* **⚡ Native Layer (C/C++):** Melakukan *inline hooking* dan *linker namespace manipulation*. Berfungsi sebagai jantung stabilisasi core.
* **🧠 Virtualization Layer (Java):** Menginterupsi `ActivityThread`, `IActivityManager` (AMS), dan `IActivityTaskManager` (ATM) untuk proses *task hijacking*.
* **🛡️ Bypass Layer:** Implementasi *Dual-Stage Hidden API Bypass* menggunakan `reflection.jar` untuk menembus proteksi internal Android 10 hingga Android 14+.

## 📁 Struktur Inti Proyek

```text
📂 BlackHook-Core
├── 📂 app/libs/             # Dependency: reflection.jar (Hidden API Bypass)
├── 📂 app/src/main/java/    # Manager Engine & Proxy Logic (AMS, PMS, Instrumentation)
├── 📂 app/src/main/jni/     # Native C++ Source & Linker Optimization
└── 📂 .github/workflows/    # CI/CD: Automated AAR Compilation
```
---
## 🚀 Fitur Unggulan (Updated)

- [x] **Dynamic Instrumentation Swapping**: Menukar *lifecycle activity* target secara *real-time* melalui `VInstrumentation`.
- [x] **AMS/ATM Virtualization**: *Bypass* verifikasi sistem untuk meluncurkan APK target tanpa instalasi formal.
- [x] **Stealth Bootstrap**: Inisialisasi *engine* melalui *native bridge* untuk menghindari deteksi *runtime signature*.
- [x] **Multi-Process Mapping**: Sinkronisasi *engine* antara proses *host* dan `:virtual_process`.
- [x] **Automated Build CI/CD**: Menghasilkan AAR siap pakai untuk setiap *commit* melalui GitHub Actions.



### 🚀 Core Capabilities
| Feature | Description | Status |
| :--- | :--- | :--- |
|  **AMS/ATM Layer** | Task Hijacking & Activity Swapping | ![Ready](https://img.shields.io/badge/-Ready-success) |
|  **Reflective Hook** | Hidden API Bypass & Method Interception | ![Armed](https://img.shields.io/badge/-Armed-blue) |
|  **Native Core** | Linker Namespace & Memory Bridge | ![Stable](https://img.shields.io/badge/-Stable-brightgreen) |
|  **V-Sandbox** | Isolated Process Mapping (:virtual_process) | ![Active](https://img.shields.io/badge/-Active-orange) |

---

## 🛠️ Alur Kerja Engine (Workflow)

1. **Bootstrap**: Memuat `libvphone_core.so` dan melakukan *bypass* Hidden API.
2. **Identity Setup**: Melakukan *hooking* pada `IPackageManager` untuk menyuntikkan metadata APK target ke sistem.
3. **Bridge Intent**: Mengalihkan *intent* aplikasi target agar berjalan melalui `StubActivity`.
4. **Activity Swap**: `VInstrumentation` mengambil alih kendali dan melakukan *instantiation* class target langsung dari APK eksternal.

---

## ⚠️ DISCLAIMER

> **PROYEK INI DIBUAT HANYA UNTUK TUJUAN PENDIDIKAN DAN PENELITIAN KEAMANAN.**
>
> Seluruh risiko penggunaan, termasuk ketidakstabilan sistem atau pelanggaran TOS (*Terms of Service*) aplikasi pihak ketiga, ditanggung sepenuhnya oleh pengguna. Pengembang tidak bertanggung jawab atas kerugian yang ditimbulkan dari penggunaan alat ini.

---

<p align="center">
  <b>Developed with ❤️ by geterrorcode-me</b><br>
  <i>"Intercept the lifecycle, dominate the framework."</i>
</p>

---


