package com.aura.bridge

import android.graphics.Color
import android.os.Bundle
import android.widget.FrameLayout
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {

    private lateinit var bridge: AuraBridge

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        bridge = AuraBridge()

        // Root Layout
        val root = FrameLayout(this).apply { setBackgroundColor(Color.BLACK) }

        // Tampilkan Liquid Island yang asli via SurfaceView
        val liquidIslandView = AuraSurfaceView(this, bridge)
        root.addView(liquidIslandView)

        // Overlay Teks Status
        val statusText =
                TextView(this).apply {
                    setTextColor(Color.WHITE)
                    textSize = 14f
                    setPadding(50, 50, 50, 50)
                    text = "Aura OS | Privacy Shield ACTIVE"
                }

        root.addView(statusText)
        setContentView(root)

        // Inisialisasi Bridge
        try {
            bridge.initializeBridge()
        } catch (e: Exception) {
            statusText.text = "Aura OS Error: Native Bridge Not Ready"
        }
    }
}
