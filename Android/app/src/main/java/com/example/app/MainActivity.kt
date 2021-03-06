package com.example.app

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.RectF
import android.os.Build
import android.os.Bundle
import android.view.ViewGroup
import androidx.activity.viewModels
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
import androidx.lifecycle.Observer
import com.example.app.databinding.MainActivityBinding
import org.tensorflow.lite.support.common.FileUtil
import java.util.concurrent.Executors
import kotlin.math.min
import kotlin.random.Random

class MainActivity: AppCompatActivity() {
    private val LABELS_PATH = "coco_ssd_mobilenet_v1_1.0_labels.txt"

    private lateinit var activityCameraBinding: MainActivityBinding
    private val vm: MainViewModel by viewModels()
    private val permissions = listOf(Manifest.permission.CAMERA)
    private val permissionsRequestCode = Random.nextInt(0, 10000)
    private val executor = Executors.newSingleThreadExecutor()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        activityCameraBinding = MainActivityBinding.inflate(layoutInflater)
        setContentView(activityCameraBinding.root)

        vm.tvString.observe(this, Observer {
            activityCameraBinding.tvResult.text = it
        })
        vm.rect.observe(this) {
            val location = mapOutputCoordinates(it)
            (activityCameraBinding.boxPrediction).apply {
                val currentLayoutParams = layoutParams as ViewGroup.MarginLayoutParams

                currentLayoutParams.topMargin = location.top.toInt()
                currentLayoutParams.leftMargin = location.left.toInt()
                currentLayoutParams.width = min(activityCameraBinding.viewFinder.width, location.right.toInt() - location.left.toInt())
                currentLayoutParams.height = min(activityCameraBinding.viewFinder.height, location.bottom.toInt() - location.top.toInt())

                layoutParams = currentLayoutParams
            }
        }
        vm.tvBenchmark.observe(this, Observer {
            activityCameraBinding.tvBenchmark.text = it
        })
        activityCameraBinding.btnSave.setOnClickListener {
            vm.saveButtonClickEvent()
        }

        vm.setLabels(FileUtil.loadLabels(this, LABELS_PATH))
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

    override fun onDestroy() {
        super.onDestroy()
        vm.destory()
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

            imageAnalysis.setAnalyzer(executor, ImageAnalysis.Analyzer { image ->
                vm.process(image, resources.assets)
            })

            // Create a new camera selector each time, enforcing lens facing
            val cameraSelector = CameraSelector.Builder().requireLensFacing(CameraSelector.LENS_FACING_BACK).build()

            // Apply declared configs to CameraX using the same lifecycle owner
            cameraProvider.unbindAll()
            val camera = cameraProvider.bindToLifecycle(
                this as LifecycleOwner, cameraSelector, preview, imageAnalysis)

            // Use the camera object to link our preview use case with the view
            preview.setSurfaceProvider(activityCameraBinding.viewFinder.surfaceProvider)

        }, ContextCompat.getMainExecutor(this))
    }

    /** Convenience method used to check if all permissions required by this app are granted */
    private fun hasPermissions(context: Context) = permissions.all {
        ContextCompat.checkSelfPermission(context, it) == PackageManager.PERMISSION_GRANTED
    }

    private fun mapOutputCoordinates(location: RectF): RectF {
        // Step 1: map location to the preview coordinates
        val previewLocation = RectF(
            location.left * activityCameraBinding.viewFinder.width,
            location.top * activityCameraBinding.viewFinder.height,
            location.right * activityCameraBinding.viewFinder.width,
            location.bottom * activityCameraBinding.viewFinder.height
        )
        val correctedLocation = previewLocation

        // Step 3: compensate for 1:1 to 4:3 aspect ratio conversion + small margin
        val margin = 0.1f
        val requestedRatio = 4f / 3f
        val midX = (correctedLocation.left + correctedLocation.right) / 2f
        val midY = (correctedLocation.top + correctedLocation.bottom) / 2f
        return if (activityCameraBinding.viewFinder.width < activityCameraBinding.viewFinder.height) {
            RectF(
                midX - (1f + margin) * requestedRatio * correctedLocation.width() / 2f,
                midY - (1f - margin) * correctedLocation.height() / 2f,
                midX + (1f + margin) * requestedRatio * correctedLocation.width() / 2f,
                midY + (1f - margin) * correctedLocation.height() / 2f
            )
        } else {
            RectF(
                midX - (1f - margin) * correctedLocation.width() / 2f,
                midY - (1f + margin) * requestedRatio * correctedLocation.height() / 2f,
                midX + (1f - margin) * correctedLocation.width() / 2f,
                midY + (1f + margin) * requestedRatio * correctedLocation.height() / 2f
            )
        }
    }
}