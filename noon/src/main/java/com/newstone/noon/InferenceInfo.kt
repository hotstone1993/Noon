package com.newstone.noon

data class InferenceInfo(
    val model: ByteArray,
    val modelSize: Int,
    val type: Int,
    val mlInfo: MLInfo,
    val input: InputInfo,
    val output: OutputInfo
) {
    enum class InferenceType {
        IMAGE, AUDIO
    }
    enum class InferenceDelegate {
        CPU, GPU
    }
    data class InputInfo(
        var shape: List<Int>
    )
    data class OutputInfo(
        var shape: List<Int>? = null
    )
}


