package com.newstone.noon

class Noon{
    private var nativeInstance: Long = 0

    external fun setup(info: InferenceInfo): Int
    external fun inference(input: ByteArray, output: FloatArray): Int
    external fun destroy()
    external fun saveImage()
    external fun getBenchmark(): String

    companion object {
        init {
            System.loadLibrary("noon")
        }
    }
}