package com.aura.os.bridge

import android.accessibilityservice.AccessibilityService
import android.view.accessibility.AccessibilityEvent
import android.view.accessibility.AccessibilityNodeInfo
import android.util.Log

/**
 * @brief AuraMediatorService bertindak sebagai "The Great Mediator".
 * Menghubungkan niat pengguna Aura OS langsung ke aplikasi pihak ketiga (Gojek).
 */
class AuraMediatorService : AccessibilityService() {

    private val TAG = "AuraMediator"
    private val TARGET_PACKAGE = "com.gojek.app"

    override fun onAccessibilityEvent(event: AccessibilityEvent) {
        // Filter: Hanya proses jika aplikasi target sedang aktif
        if (event.packageName == TARGET_PACKAGE) {
            Log.d(TAG, "Aura OS mendeteksi aktivitas di: $TARGET_PACKAGE")
            
            val rootNode = rootInActiveWindow ?: return
            findAndInteractWithOrderButton(rootNode)
        }
    }

    /**
     * @brief Menelusuri pohon node untuk mencari tombol interaksi.
     */
    private fun findAndInteractWithOrderButton(rootNode: AccessibilityNodeInfo) {
        // Cari tombol dengan teks "Pesan" atau "Order" sesuai spesifikasi
        val nodes = rootNode.findAccessibilityNodeInfosByText("Pesan")
        val orderNodes = rootNode.findAccessibilityNodeInfosByText("Order")
        
        val combinedNodes = nodes + orderNodes

        if (combinedNodes.isEmpty()) {
            Log.w(TAG, "Tombol 'Pesan' atau 'Order' tidak ditemukan di layar saat ini.")
            return
        }

        for (node in combinedNodes) {
            if (node.isClickable) {
                performClickAction(node)
                break // Klik satu saja yang paling relevan
            }
        }
    }

    /**
     * @brief Mensimulasikan ketukan pengguna secara aman.
     */
    private fun performClickAction(node: AccessibilityNodeInfo) {
        try {
            val success = node.performAction(AccessibilityNodeInfo.ACTION_CLICK)
            if (success) {
                Log.i(TAG, "Aura OS berhasil memediasi aksi klik pada: ${node.text}")
            } else {
                Log.e(TAG, "Gagal mensimulasikan klik pada node.")
            }
        } catch (e: Exception) {
            // Penanganan error sesuai Master Context: Jangan biarkan crash
            Log.e(TAG, "Kesalahan saat mencoba mediasi interaksi: ${e.message}")
        }
    }

    override fun onInterrupt() {
        Log.i(TAG, "Aura Mediator Service diinterupsi oleh sistem.")
    }
}
