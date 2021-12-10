package com.example.core

import android.content.res.AssetManager

class NativeLib {

    /**
     * A native method that is implemented by the 'core' native library,
     * which is packaged with this application.
     */
    private var nativeInstance: Long = 0

    external fun create(): Int
    external fun loadModel(assetManager: AssetManager, modelPath: String, labelPath: String): Int
    external fun setup(width: Int, height: Int, pixelStride: Int): Int
    external fun inference(arr: ByteArray): Int

    companion object {
        // Used to load the 'core' library on application startup.
        init {
            System.loadLibrary("core")
        }
    }
}