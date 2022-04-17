package com.newstone.noon

data class InferenceInfo(
    var type: Int,
    var input: InputInfo,
    var output: OutputInfo
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


