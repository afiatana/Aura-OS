package com.aura.bridge

import android.content.Context
import android.view.SurfaceHolder
import android.view.SurfaceView

/** View khusus untuk menampilkan Liquid Island UI */
class AuraSurfaceView(context: Context, private val bridge: AuraBridge) :
        SurfaceView(context), SurfaceHolder.Callback {

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        // Saat layar siap, beri tahu C++ untuk mulai menggambar
        bridge.startLiquidIsland(holder.surface)
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
        // Handle perubahan ukuran layar (rotasi)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {
        // Hentikan rendering saat aplikasi ditutup
    }
}
