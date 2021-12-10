package com.example.app

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.*
import android.os.Build
import android.os.Bundle
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import androidx.camera.core.AspectRatio
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner
import com.example.app.databinding.MainActivityBinding
import com.example.core.NativeLib
import java.util.concurrent.Executors
import kotlin.random.Random

class MainActivity: AppCompatActivity() {
    private val MODEL_PATH = "coco_ssd_mobilenet_v1_1.0_quant.tflite"
    private val LABEL_PATH = "coco_ssd_mobilenet_v1_1.0_labels.txt"

    private lateinit var activityCameraBinding: MainActivityBinding

    private val executor = Executors.newSingleThreadExecutor()
    private val permissions = listOf(Manifest.permission.CAMERA)
    private val permissionsRequestCode = Random.nextInt(0, 10000)

    private var lensFacing: Int = CameraSelector.LENS_FACING_BACK

    private var pauseAnalysis = false
    private val nativeLib = NativeLib()

    private lateinit var input: ByteArray

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        activityCameraBinding = MainActivityBinding.inflate(layoutInflater)
        setContentView(activityCameraBinding.root)

        nativeLib.create()
        nativeLib.loadModel(resources.assets, MODEL_PATH, LABEL_PATH)
    }


    /** Declare and bind preview and analysis use cases */
    @RequiresApi(Build.VERSION_CODES.S)
    @SuppressLint("UnsafeExperimentalUsageError")
    private fun bindCameraUseCases() = activityCameraBinding.viewFinder.post {
        val cameraProviderFuture = ProcessCameraProvider.getInstance(this)
        cameraProviderFuture.addListener(Runnable {
            // Camera provider is now guaranteed to be available
            val cameraProvider = cameraProviderFuture.get()

            // Set up the view finder use case to display camera preview
            val preview = Preview.Builder()
                .setTargetAspectRatio(AspectRatio.RATIO_4_3)
                .setTargetRotation(activityCameraBinding.viewFinder.display.rotation)
                .build()

            // Set up the image analysis use case which will process frames in real time
            val imageAnalysis = ImageAnalysis.Builder()
                .setTargetAspectRatio(AspectRatio.RATIO_4_3)
                .setTargetRotation(activityCameraBinding.viewFinder.display.rotation)
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .setOutputImageFormat(ImageAnalysis.OUTPUT_IMAGE_FORMAT_RGBA_8888)
                .build()

            var frameCounter = 0
            var lastFpsTimestamp = System.currentTimeMillis()

            imageAnalysis.setAnalyzer(executor, ImageAnalysis.Analyzer { image ->
                // Early exit: image analysis is in paused state
                if (pauseAnalysis) {
                    image.close()
                    return@Analyzer
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
            })

            // Create a new camera selector each time, enforcing lens facing
            val cameraSelector = CameraSelector.Builder().requireLensFacing(lensFacing).build()

            // Apply declared configs to CameraX using the same lifecycle owner
            cameraProvider.unbindAll()
            val camera = cameraProvider.bindToLifecycle(
                this as LifecycleOwner, cameraSelector, preview, imageAnalysis)

            // Use the camera object to link our preview use case with the view
            preview.setSurfaceProvider(activityCameraBinding.viewFinder.surfaceProvider)

        }, ContextCompat.getMainExecutor(this))
    }

    @RequiresApi(Build.VERSION_CODES.S)
    override fun onResume() {
        super.onResume()

        // Request permissions each time the app resumes, since they can be revoked at any time
        if (!hasPermissions(this)) {
            ActivityCompat.requestPermissions(
                this, permissions.toTypedArray(), permissionsRequestCode)
        } else {
            bindCameraUseCases()
        }
    }

    @RequiresApi(Build.VERSION_CODES.S)
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == permissionsRequestCode && hasPermissions(this)) {
            bindCameraUseCases()
        } else {
            finish() // If we don't have the required permissions, we can't run
        }
    }

    /** Convenience method used to check if all permissions required by this app are granted */
    private fun hasPermissions(context: Context) = permissions.all {
        ContextCompat.checkSelfPermission(context, it) == PackageManager.PERMISSION_GRANTED
    }
}