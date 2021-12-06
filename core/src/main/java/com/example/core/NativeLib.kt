package com.example.core

class NativeLib {

    /**
     * A native method that is implemented by the 'core' native library,
     * which is packaged with this application.
     */
    private var nativeInstance: Long = 0

    external fun create(): Int
    external fun loadModel(path: String): Int
    external fun inference(arr: IntArray): Int

    companion object {
        // Used to load the 'core' library on application startup.
        init {
            System.loadLibrary("core")
        }
    }
}