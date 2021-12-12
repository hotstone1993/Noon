//
// Created by newst on 2021-12-02.
//

#include "include/Processor.h"
#include "include/SimpleAverageFilter.h"

#define INPUT_INDEX 0

#define IMAGE_COUNT_INDEX 0
#define IMAGE_WIDTH_INDEX 1
#define IMAGE_HEIGHT_INDEX 2
#define IMAGE_PIXEL_STRIDE_INDEX 3


Processor::Processor():
    processedBuffer(nullptr),
    filter(nullptr) {
}
Processor::~Processor() {
    destroy();
}

int Processor::loadModel(const char* file, size_t fileSize) {
    model = tflite::FlatBufferModel::BuildFromBuffer(file, fileSize);
    TFLITE_MINIMAL_CHECK(model != nullptr);
    builder = std::make_unique<tflite::InterpreterBuilder>(*model, resolver);
    builder->operator()(&interpreter);
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

    TfLiteTensor* tensor = interpreter->input_tensor(INPUT_INDEX);
    if(tensor->dims->size <= IMAGE_PIXEL_STRIDE_INDEX) {
        return FAIL;
    }

    ImageInfo targetInfo{
        tensor->dims->data[IMAGE_WIDTH_INDEX],
        tensor->dims->data[IMAGE_HEIGHT_INDEX],
        tensor->dims->data[IMAGE_PIXEL_STRIDE_INDEX]
    };

    if(processedBuffer == nullptr) {
        processedBuffer = new uint8_t[targetInfo.width * targetInfo.height * targetInfo.pixelStride];
        memset(processedBuffer, 0, sizeof(int8_t) * targetInfo.width * targetInfo.height * targetInfo.pixelStride);
    } else {
        return FAIL;
    }
    if(filter == nullptr) {
        filter = new SimpleAverageFilter<uint8_t>();
    }
    filter->setup(inputInfo, targetInfo);

    return SUCCESS;
}

int Processor::inference(uint8_t* inputBuffer, float* output) {
    filter->process(inputBuffer, processedBuffer);

    uint8_t* tensorInput = interpreter->typed_tensor<uint8_t>(interpreter->inputs()[INPUT_INDEX]);
    size_t size = filter->getTargetInfo().width * filter->getTargetInfo().height * filter->getTargetInfo().pixelStride;
    memcpy(tensorInput, processedBuffer, sizeof(uint8_t) * size);

    // Run inference
    TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);

    std::vector<int> outputIndices = interpreter->outputs();
    for(int i = 0; i < outputIndices.size(); ++i) {
        output[i] = interpreter->typed_output_tensor<float>(i)[0];
    }

    return SUCCESS;
}

////////////////////////////////////////////////////////////////////////////

int Processor::destroy() {
    DELETE_ARRAY(processedBuffer)
    DELETE(filter)

    return SUCCESS;
}