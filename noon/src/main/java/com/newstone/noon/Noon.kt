package com.newstone.noon

import android.content.res.AssetManager

class Noon {
    private var nativeInstance: Long = 0

    external fun create(): Int
    external fun loadModel(assetManager: AssetManager, modelPath: String): Int
    external fun setup(info: InferenceInfo): Int
    external fun inference(input: ByteArray, output: FloatArray): Int
    external fun saveImage()
    external fun getBenchmark(): String

    companion object {
        init {
            System.loadLibrary("noon")
        }
    }
}