package com.newstone.noon

data class MLInfo(
    val mode: Int,
    val delegate: Int,
    val tfl: TFLInfo? = null
) {
    enum class MlMode {
        TENSORFLOW_LITE, PYTORCH
    }
    data class TFLInfo(
        val numThread: Int = -1,
        val allowFp16PrecisionForFp32: Boolean = false
    )
}