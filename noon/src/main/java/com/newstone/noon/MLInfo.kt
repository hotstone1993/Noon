package com.newstone.noon

data class MLInfo(
    val mode: Int,
    val delegate: Int,
    val info: BaseInfo? = null
) {
    enum class MlMode {
        TENSORFLOW_LITE, PYTORCH
    }

    interface BaseInfo

    data class TFLInfo(
        val numThread: Int = -1,
        val allowFp16PrecisionForFp32: Boolean = false
    ): BaseInfo
}