package com.newstone.noon

class Noon{
    private var nativeInstance: Long = 0

    external fun setup(info: InferenceInfo): Int
    fun inference(input: ByteArray, output: Array<Array<Float>>): Int {
        val tempArray = output.flatten().toFloatArray()
        val result = nativeInference(input, tempArray)

        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    private external fun nativeInference(input: ByteArray, output: FloatArray): Int
    external fun destroy()
    external fun saveImage()
    external fun getBenchmark(): String

    companion object {
        init {
            System.loadLibrary("noon")
        }
    }
}