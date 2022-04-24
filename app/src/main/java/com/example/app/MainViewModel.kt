package com.example.app

import android.content.res.AssetManager
import android.graphics.RectF
import androidx.camera.core.*
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import com.newstone.noon.InferenceInfo
import com.newstone.noon.Noon

class MainViewModel: ViewModel() {
    private val MODEL_PATH = "coco_ssd_mobilenet_v1_1.0_quant.tflite"
    private val ACCURACY_THRESHOLD = 0.5

    private var pauseAnalysis = false
    private val nativeLib = Noon()

    private lateinit var input: ByteArray
    private lateinit var output: Array<Array<Float>>

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
            output = arrayOf(Array(40, {0f}), Array(10, {0f}), Array(10, {0f}), Array(1, {0f}))
            val model = getModel(assetManager)
            val modelSize = model.size
            val inferenceInfo = InferenceInfo(
                type = InferenceInfo.InferenceType.IMAGE.ordinal,
                delegate = InferenceInfo.InferenceDelegate.CPU.ordinal,
                model = getModel(assetManager),
                modelSize = modelSize,
                input = InferenceInfo.InputInfo(
                    shape = listOf(image.width, image.height, pixelStride)
                ),
                output = InferenceInfo.OutputInfo(
                    shape = listOf(10)
                )
            )
            nativeLib.setup(inferenceInfo)
        }

        image.planes.first().buffer.get(input, 0, image.width * image.height * pixelStride)
        nativeLib.inference(input, output)
        if (ACCURACY_THRESHOLD <= output[2][0]) {
            tvString.postValue("${labels[output[1][0].toInt() + 1]}: ${output[2][0]}")
            rect.postValue(RectF(output[0][1], output[0][0], output[0][3], output[0][2]))
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