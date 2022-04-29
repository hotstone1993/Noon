package com.newstone.noon

class Noon{
    private var nativeInstance: Long = 0

    external fun setup(info: InferenceInfo): Int

    fun getOutput(output: Array<Array<Long>>): Int {
        val tempArray = output.flatten().toLongArray()
        val result = nativeGetLongArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<ULong>>): Int {
        val tempArray = output.flatten().toULongArray()
        val result = nativeGetULongArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<Float>>): Int {
        val tempArray = output.flatten().toFloatArray()
        val result = nativeGetFloatArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<Int>>): Int {
        val tempArray = output.flatten().toIntArray()
        val result = nativeGetIntArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<UInt>>): Int {
        val tempArray = output.flatten().toUIntArray()
        val result = nativeGetUIntArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<Short>>): Int {
        val tempArray = output.flatten().toShortArray()
        val result = nativeGetShortArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<UShort>>): Int {
        val tempArray = output.flatten().toUShortArray()
        val result = nativeGetUShortArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<Byte>>): Int {
        val tempArray = output.flatten().toByteArray()
        val result = nativeGetByteArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }
    fun getOutput(output: Array<Array<UByte>>): Int {
        val tempArray = output.flatten().toUByteArray()
        val result = nativeGetUByteArrayOutput(tempArray)
        var idx = 0
        for (c in 0 until output.size) {
            for (r in 0 until output[c].size) {
                output[c][r] = tempArray[idx++]
            }
        }

        return result
    }

    external fun inferenceULongArray(input: LongArray): Int
    external fun inferenceLongArray(input: LongArray): Int
    external fun inferenceDoubleArray(input: DoubleArray): Int
    external fun inferenceFloatArray(input: FloatArray): Int
    external fun inferenceIntArray(input: IntArray): Int
    external fun inferenceUIntArray(input: IntArray): Int
    external fun inferenceShortArray(input: ShortArray): Int
    external fun inferenceUShortArray(input: ShortArray): Int
    external fun inferenceByteArray(input: ByteArray): Int
    external fun inferenceUByteArray(input: ByteArray): Int

    private external fun nativeGetULongArrayOutput(output: ULongArray): Int
    private external fun nativeGetLongArrayOutput(output: LongArray): Int
    private external fun nativeGetDoubleArrayOutput(output: DoubleArray): Int
    private external fun nativeGetFloatArrayOutput(output: FloatArray): Int
    private external fun nativeGetIntArrayOutput(output: IntArray): Int
    private external fun nativeGetUIntArrayOutput(output: UIntArray): Int
    private external fun nativeGetShortArrayOutput(output: ShortArray): Int
    private external fun nativeGetUShortArrayOutput(output: UShortArray): Int
    private external fun nativeGetByteArrayOutput(output: ByteArray): Int
    private external fun nativeGetUByteArrayOutput(output: UByteArray): Int

    external fun destroy()
    external fun saveImage()
    external fun getBenchmark(): String

    companion object {
        init {
            System.loadLibrary("noon")
        }
    }
}