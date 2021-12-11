package com.example.app

import android.content.res.AssetManager
import androidx.camera.core.*
import androidx.lifecycle.ViewModel
import com.example.core.NativeLib

class MainViewModel: ViewModel() {
    private val MODEL_PATH = "coco_ssd_mobilenet_v1_1.0_quant.tflite"
    private val LABEL_PATH = "coco_ssd_mobilenet_v1_1.0_labels.txt"

    private var pauseAnalysis = false
    private val nativeLib = NativeLib()

    private lateinit var input: ByteArray

    private var frameCounter = 0
    private var lastFpsTimestamp = System.currentTimeMillis()

    fun initNatvieLibrary(assetManager: AssetManager) {
        nativeLib.create()
        nativeLib.loadModel(assetManager, MODEL_PATH, LABEL_PATH)
    }

    fun process(image: ImageProxy) {
        if (pauseAnalysis) {
            image.close()
        }

        val pixelStride = image.planes.first().pixelStride
        if(!::input.isInitialized) {
            input = ByteArray(image.width * image.height * pixelStride)
            nativeLib.setup(width = image.width, height = image.height, pixelStride = pixelStride)
        }

        image.planes.first().buffer.get(input, 0, image.width * image.height * pixelStride)
        nativeLib.inference(input)

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
}