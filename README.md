# Noon
Noon is an SDK that makes it easy to infer TensorFlow-Lite and PyTorch(not yet supported) Models.    
Since it is implemented based on *C++*, it can be used on various platforms.

## Inference the Tensorflow-Lite model on Android
<pre>
<code>
    // First, Create Noon instance.
    private val nativeLib = Noon()
    private var outputs = mutableListOf<FloatArray>() 
    
    // The model(*.tflite) is read from the asset folder.
    val model = getModel(assetManager) 
    val modelSize = model.size
    // InferenceInfo
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
            // set an input shape for the data to be put as input.
            shape = listOf(image.width, image.height, pixelStride)
        ),
        // If you do not want to transform into a specific shape, you should not pass a value.
        output = InferenceInfo.OutputInfo()
    )
    nativeLib.setup(inferenceInfo)
    // allocate output buffer
    for (i in 0 until nativeLib.getOutputArraySize()) {
        outputs.add(nativeLib.allocFloatBuffer(i, false))
    }
    // inference input
    nativeLib.inferenceUByteArray(input)
    // and get output(result).
    for (i in 0 until nativeLib.getOutputArraySize()) {
        nativeLib.nativeGetFloatArrayOutput(i, outputs[i])
    }
</code>
</pre>

## Remaining Work
- Currently only available for Android.    
  (The build system will be completely improved so that it can be built with libraries of various platforms such as iOS, Mac, Windows, etc.)
- Need to improve pre- and post-processing functions.
- It is necessary to provide various example apps.
- No PyTorch model inference capabilities.
  (TensorFlow-Lite library only supports arm64-v8a {6e3666c9})
