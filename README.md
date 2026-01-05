# Aura OS - The Private, Fluid Computing Platform

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Build Status](https://github.com/afiatana/Aura-OS/actions/workflows/main.yml/badge.svg)

**Aura OS** is a next-generation experimental operating system layer focused on **On-Device Privacy**, **Liquid Island UI**, and **Humble AI**. It is built using a hybrid architecture that combines the safety of Rust with the performance of C++/Vulkan.

---

## 🌌 Core Pillars

1.  **On-Device Privacy (Rust Kernel)**: All sensitive data processing happens within the Rust-based supervisor. Identity is local-first, and data is masked before any interaction.
2.  **Liquid Island UI (C++/Vulkan)**: A high-performance, gooey, fluid interface inspired by organic movements. Powered by raw Vulkan shaders.
3.  **Humble AI (Mojo)**: Integrated on-device intelligence that assists the user without intrusive data collection.
4.  **Hardware Bridge (Android/Linux)**: A flexible bridge that allows Aura OS components to run on existing hardware drivers while maintaining logical independence.

---

## 🚀 Current Project Status

| Component | Technology | Status |
|-----------|------------|--------|
| **Kernel Core** | Rust 2024 | ✅ Beta (Privacy Shield Active) |
| **Graphics Engine** | C++20 / Vulkan | ✅ Prototype (Liquid Shaders) |
| **AI Core** | Mojo | 🏗️ Initializing (via Pixi) |
| **App Bridge** | Kotlin / JNI | 🏗️ Structure Ready |

**Note:** Aura OS is currently in **Pre-Alpha**. It is a development laboratory and is not yet ready for standalone "hardware-only" installation. It currently runs as a layer on top of Linux (WSL2) and Android.

---

## 🛠️ Optimization Features Added

- **Privacy Shield v1.0**: On-device data masking in the Rust Kernel.
- **CI/CD Integration**: Automatic build checks via GitHub Actions.
- **Smart FFI**: Optimized communication between Rust and C++ modules.
- **Hybrid Memory Management**: Logic-based memory safety inherited from Rust.

---

## 👨‍💻 Development & Contribution

We welcome contributors! Here is how to get started:

### Prerequisites
- **Rust** (1.75+)
- **CMake** (3.20+)
- **Vulkan SDK**
- **Android Studio** (for Bridge development)
- **Pixi** (for Mojo AI development)

### Build Instructions
1.  **Kernel**: `cd aura-kernel && cargo build`
2.  **Graphics**: `cd aura-graphics && cmake . -B build`
3.  **Bridge**: Open `aura-bridge` in Android Studio.

---

## 📦 How to "Install"

Since Aura OS is an experimental layer:
1.  **For Android**: Build the `aura-bridge` APK and install it on your device.
2.  **For Desktop**: Run the `aura-graphics` executable to experience the Liquid Island environment.
3.  **For AI**: Activate the Mojo environment via `pixi shell`.

---

## 📄 License
This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

Developed with ❤️ by **Afiatana** and the Aura Community.
