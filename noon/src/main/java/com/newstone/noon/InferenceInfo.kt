package com.newstone.noon

data class InferenceInfo(
    val type: Int,
    val model: ByteArray,
    val modelSize: Int,
    val input: InputInfo,
    val output: OutputInfo
) {
    enum class InferenceType {
        IMAGE, AUDIO
    }
    data class InputInfo(
        var shape: List<Int>
    )
    data class OutputInfo(
        var shape: List<Int>
    )
}


