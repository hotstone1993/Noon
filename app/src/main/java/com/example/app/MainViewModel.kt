package com.example.app

import android.content.res.AssetManager
import android.graphics.RectF
import androidx.camera.core.*
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import com.newstone.noon.InferenceInfo
import com.newstone.noon.MLInfo
import com.newstone.noon.Noon

class MainViewModel: ViewModel() {
    private val MODEL_PATH = "coco_ssd_mobilenet_v1_1.0_quant.tflite"
    private val ACCURACY_THRESHOLD = 0.5

    private var pauseAnalysis = false
    private val nativeLib = Noon()

    private lateinit var input: ByteArray
    private lateinit var positionOutput: FloatArray
    private lateinit var labelIdxOutput: FloatArray
    private lateinit var confidenceOutput: FloatArray

    private var frameCounter = 0
    private var lastFpsTimestamp = System.currentTimeMillis()

    private lateinit var labels: List<String>
    val tvString = MutableLiveData<String>()
    val rect = MutableLiveData<RectF>()
    val tvBenchmark = MutableLiveData<String>()

    fun setLabels(labels: List<String>) {
        this.labels = labels
    }

    private fun getModel(assetManager: AssetManager): ByteArray {
        return assetManager.open(MODEL_PATH).readBytes()
    }

    fun process(image: ImageProxy, assetManager: AssetManager) {
        if (pauseAnalysis) {
            image.close()
        }

        val pixelStride = image.planes.first().pixelStride
        if(!::input.isInitialized) {
            input = ByteArray(image.width * image.height * pixelStride, {0})
            positionOutput = FloatArray(4 * 10)
            labelIdxOutput = FloatArray(10)
            confidenceOutput = FloatArray(10)
            val model = getModel(assetManager)
            val modelSize = model.size
            val inferenceInfo = InferenceInfo(
                type = InferenceInfo.InferenceType.IMAGE.ordinal,
                mlInfo = MLInfo(
                    mode = MLInfo.MlMode.TENSORFLOW_LITE.ordinal,
                    delegate = InferenceInfo.InferenceDelegate.CPU.ordinal,
                    info = MLInfo.TFLInfo(
                        numThread = 4,
                        allowFp16PrecisionForFp32 = false
                    )
                ),
                model = model,
                modelSize = modelSize,
                input = InferenceInfo.InputInfo(
                    shape = listOf(image.width, image.height, pixelStride)
                ),
                output = InferenceInfo.OutputInfo()
            )
            nativeLib.setup(inferenceInfo)
        }

        image.planes.first().buffer.get(input, 0, image.width * image.height * pixelStride)
        nativeLib.inferenceUByteArray(input)
        nativeLib.nativeGetFloatArrayOutput(0, positionOutput)
        nativeLib.nativeGetFloatArrayOutput(1, labelIdxOutput)
        nativeLib.nativeGetFloatArrayOutput(2, confidenceOutput)
        if (ACCURACY_THRESHOLD <= confidenceOutput[0]) {
            tvString.postValue("${labels[labelIdxOutput[0].toInt() + 1]}: ${confidenceOutput[0]}")
            rect.postValue(RectF(positionOutput[1], positionOutput[0], positionOutput[3], positionOutput[2]))
        } else {
            tvString.postValue("")
            rect.postValue(RectF(0.0f, 0.0f, 0.0f, 0.0f))
        }
        tvBenchmark.postValue(nativeLib.getBenchmark())

        // Compute the FPS of the entire pipeline\
        val frameCount = 10
        if (++frameCounter % frameCount == 0) {
            frameCounter = 0
            val now = System.currentTimeMillis()
            val delta = now - lastFpsTimestamp
            val fps = 1000 * frameCount.toFloat() / delta
            lastFpsTimestamp = now
        }

        image.close()
    }

    fun saveButtonClickEvent() {
        nativeLib.saveImage()
    }

    fun destory() {
        nativeLib.destroy()
    }
}