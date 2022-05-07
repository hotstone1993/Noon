package com.newstone.noon

class Noon{
    private var nativeInstance: Long = 0

    external fun setup(info: InferenceInfo): Int

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

    external fun nativeGetULongArrayOutput(idx: Int, output: ULongArray): Int
    external fun nativeGetLongArrayOutput(idx: Int, output: LongArray): Int
    external fun nativeGetDoubleArrayOutput(idx: Int, output: DoubleArray): Int
    external fun nativeGetFloatArrayOutput(idx: Int, output: FloatArray): Int
    external fun nativeGetIntArrayOutput(idx: Int, output: IntArray): Int
    external fun nativeGetUIntArrayOutput(idx: Int, output: UIntArray): Int
    external fun nativeGetShortArrayOutput(idx: Int, output: ShortArray): Int
    external fun nativeGetUShortArrayOutput(idx: Int, output: UShortArray): Int
    external fun nativeGetByteArrayOutput(idx: Int, output: ByteArray): Int
    external fun nativeGetUByteArrayOutput(idx: Int, output: UByteArray): Int

    external fun destroy()
    external fun saveImage()
    external fun getBenchmark(): String

    companion object {
        init {
            System.loadLibrary("noon")
        }
    }
}