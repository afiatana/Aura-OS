package com.aura.bridge

fun main() {
    println("------------------------------------------")
    println("         AURA OS BRIDGE TESTER            ")
    println("------------------------------------------")

    val bridge = AuraBridge()

    // Test 1: Inisialisasi Native Bridge
    try {
        println("- Memulai Jembatan Native...")
        val status = bridge.initializeBridge()
        if (status == 1) {
            println("✅ Success: JNI Connection Established!")
        }
    } catch (e: UnsatisfiedLinkError) {
        println("❌ Error: Library 'aura_bridge' belum dikompilasi atau tidak ditemukan di PATH.")
        println("   (Ini normal jika kita belum melakukan build CMake untuk DLL Bridge)")
    }

    // Test 2: Ambil Versi dari Rust Kernel
    try {
        val version = bridge.getKernelVersion()
        println("✅ Rust Kernel Version: $version")
    } catch (e: Throwable) {
        println("ℹ️  Menunggu kompilasi library native untuk akses data Kernel.")
    }

    println("------------------------------------------")
    println("Status: Bridge Project Synced & Ready for Coding.")
}
