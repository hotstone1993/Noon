//
// Created by newst on 2021-12-02.
//

#include "include/Processor.h"
#include "include/SimpleAverageFilter.h"

#define IMAGE_COUNT_INDEX 0
#define IMAGE_WIDTH_INDEX 1
#define IMAGE_HEIGHT_INDEX 2
#define IMAGE_PIXEL_STRIDE_INDEX 3


Processor::Processor():
    processedBuffer(nullptr),
    outputBuffer(nullptr),
    filter(nullptr) {
}
Processor::~Processor() {
    destroy();
}

int Processor::loadModel(const char* file, size_t fileSize) {
    std::unique_ptr<tflite::FlatBufferModel> model =
            tflite::FlatBufferModel::BuildFromBuffer(file, fileSize);
    TFLITE_MINIMAL_CHECK(model != nullptr);

    // Build the interpreter with the InterpreterBuilder.
    // Note: all Interpreters should be built with the InterpreterBuilder,
    // which allocates memory for the Interpreter and does various set up
    // tasks so that the Interpreter can read the provided model.
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    builder(&interpreter);
    TFLITE_MINIMAL_CHECK(interpreter != nullptr);

    // Allocate tensor buffers.
    TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
    printf("=== Pre-invoke Interpreter State ===\n");
    tflite::PrintInterpreterState(interpreter.get());

    return SUCCESS;
}


int Processor::setup(int width, int height, int pixelStride) {
    if(width == 0 || height == 0 || pixelStride == 0) {
        return FAIL;
    }

    ImageInfo inputInfo{
        width,
        height,
        pixelStride
    };

    size_t outputSize = interpreter->outputs().size();
    if(outputBuffer == nullptr) {
        outputBuffer = new float[outputSize];
        memset(outputBuffer, 0.0f, sizeof(float) * outputSize);
    } else {
        return FAIL;
    }

    TfLiteTensor* tensor = interpreter->input_tensor(0);
    if(tensor->dims->size <= IMAGE_PIXEL_STRIDE_INDEX) {
        return FAIL;
    }

    ImageInfo targetInfo{
        tensor->dims->data[IMAGE_WIDTH_INDEX],
        tensor->dims->data[IMAGE_HEIGHT_INDEX],
        tensor->dims->data[IMAGE_PIXEL_STRIDE_INDEX]
    };

    if(processedBuffer == nullptr) {
        processedBuffer = new int8_t[targetInfo.width * targetInfo.height * targetInfo.pixelStride];
        memset(processedBuffer, 0, sizeof(int8_t) * targetInfo.width * targetInfo.height * targetInfo.pixelStride);
    } else {
        return FAIL;
    }
    if(filter == nullptr) {
        filter = new SimpleAverageFilter<int8_t>();
    }
    filter->setup(inputInfo, targetInfo);

    return SUCCESS;
}

int Processor::inference(int8_t* inputBuffer, int size) {
    filter->process(inputBuffer, processedBuffer);

    auto input = interpreter->typed_input_tensor<int8_t>(0);
    input = processedBuffer;

    // Run inference
    TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);

    std::vector<int> outputIndices = interpreter->outputs();
    for(int i = 0; i < outputIndices.size(); ++i) {
        outputBuffer[i] = interpreter->typed_output_tensor<float>(i)[0];
    }

    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////

int Processor::destroy() {
    DELETE_ARRAY(processedBuffer)
    DELETE_ARRAY(outputBuffer)
    DELETE(filter)

    return SUCCESS;
}