//
// Created by newst on 2021-12-02.
//

#include "include/Processor.h"
#include "include/SimpleAverageFilter.h"
#include "lodepng.h"
#include <android/log.h>
#include <chrono>

#define INPUT_INDEX 0

#define IMAGE_COUNT_INDEX 0
#define IMAGE_WIDTH_INDEX 1
#define IMAGE_HEIGHT_INDEX 2
#define IMAGE_PIXEL_STRIDE_INDEX 3

const char* const TAG = "Noon";

#define TFLITE_MINIMAL_CHECK(x)                              \
if (!(x)) { \
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Error at %s:%d\n", __FILE__, __LINE__);                                 \
    return PROCESSOR_FAIL; \
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
Processor<INTPUT_TYPE, OUTPUT_TYPE>::Processor():
    processedBuffer(nullptr),
    filter(nullptr),
    modelBuffer(nullptr),
    delegate(nullptr) {
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
Processor<INTPUT_TYPE, OUTPUT_TYPE>::~Processor() {
    destroy();
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
int Processor<INTPUT_TYPE, OUTPUT_TYPE>::loadModel(const char* file, size_t fileSize) {
    modelBuffer = new char[fileSize];
    memcpy(modelBuffer, file, sizeof(char) * fileSize);

    model = tflite::FlatBufferModel::VerifyAndBuildFromBuffer(modelBuffer, fileSize);
    TFLITE_MINIMAL_CHECK(model != nullptr);
    builder = std::make_unique<tflite::InterpreterBuilder>(*model, resolver);
    builder->operator()(&interpreter);
    TFLITE_MINIMAL_CHECK(interpreter != nullptr);

    TfLiteGpuDelegateOptionsV2 option = TfLiteGpuDelegateOptionsV2Default();
    delegate = TfLiteGpuDelegateV2Create(&option);
    TFLITE_MINIMAL_CHECK(interpreter->ModifyGraphWithDelegate(delegate) == kTfLiteOk);

//    TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
    printf("=== Pre-invoke Interpreter State ===\n");
    tflite::PrintInterpreterState(interpreter.get());

    return PROCESSOR_SUCCESS;
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
int Processor<INTPUT_TYPE, OUTPUT_TYPE>::setup(const std::vector<int>& shape) {
    for (const int& dim : shape) {
        if (dim == 0) {
            return NOT_INITIALIZED;
        }
    }

    ImageInfo inputInfo{
            shape[0],
            shape[1],
            shape[2]
    };

    TfLiteTensor* tensor = interpreter->input_tensor(INPUT_INDEX);
    if(tensor->dims->size <= IMAGE_PIXEL_STRIDE_INDEX) {
        return PROCESSOR_FAIL;
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
        return PROCESSOR_FAIL;
    }
    if(filter == nullptr) {
        filter = new SimpleAverageFilter<uint8_t>();
    }
    filter->setup(inputInfo, targetInfo);

    return PROCESSOR_SUCCESS;
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
int Processor<INTPUT_TYPE, OUTPUT_TYPE>::inference(INTPUT_TYPE* inputBuffer, OUTPUT_TYPE* output) {
    filter->process(inputBuffer, processedBuffer);

    size_t size = filter->getTargetInfo().width * filter->getTargetInfo().height * filter->getTargetInfo().pixelStride;
    memcpy(interpreter->typed_input_tensor<uint8_t>(0), processedBuffer, sizeof(uint8_t) * size);

    // Run inference
    TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);

    std::vector<int> outputIndices = interpreter->outputs();
    int idx = 0;
    for(int i = 0; i < outputIndices.size(); ++i) {
        if(i == 0) {
            for(int j = 0; j < 4; ++j) {
                output[idx++] = interpreter->typed_tensor<float>(outputIndices[i])[j];
            }
        } else {
            output[idx++] = interpreter->typed_tensor<float>(outputIndices[i])[0];
        }
    }

    return PROCESSOR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
int Processor<INTPUT_TYPE, OUTPUT_TYPE>::destroy() {
    DELETE_ARRAY(processedBuffer)
    DELETE_ARRAY(modelBuffer)
    DELETE(filter)
    TfLiteGpuDelegateV2Delete(delegate);

    return PROCESSOR_SUCCESS;
}