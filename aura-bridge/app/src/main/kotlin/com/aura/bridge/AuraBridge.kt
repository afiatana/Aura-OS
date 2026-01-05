package com.aura.bridge

/**
 * Aura Bridge: Jembatan penghubung antara UI Android (Kotlin) dengan Kernel (Rust) dan Graphics
 * Engine (C++).
 */
class AuraBridge {

    companion object {
        init {
            System.loadLibrary("aura_bridge")
        }
    }

    /** Inisialisasi sistem Aura dari sisi mobile. */
    external fun initializeBridge(): Int

    /** Mendapatkan versi kernel yang sedang berjalan. */
    external fun getKernelVersion(): String

    /** Memulai rendering Liquid Island pada surface yang diberikan. */
    external fun startLiquidIsland(surface: Any)

    /** Callback dari AI Core untuk mengupdate animasi. */
    fun onAiUpdate(intensity: Float) {
        println("Aura Bridge: AI Intensity received -> $intensity")
    }
}
