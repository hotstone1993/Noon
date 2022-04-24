//
// Created by newst on 2021-12-02.
//

#include "include/Processor.h"
#include "include/SimpleAverageFilter.h"
#include "lodepng.h"
#include <android/log.h>
#include <chrono>


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
    delegate(nullptr),
    inputInfo(nullptr),
    targetInfo(nullptr) {
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
Processor<INTPUT_TYPE, OUTPUT_TYPE>::~Processor() {
    destroy();
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
int Processor<INTPUT_TYPE, OUTPUT_TYPE>::loadModel(const int8_t* file, size_t fileSize) {
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
    inputInfo = new BaseInfo();
    targetInfo = new BaseInfo();

    if (inputInfo == nullptr || targetInfo == nullptr) {
        return NOT_INITIALIZED;
    }

    inputInfo->nodes.push_back({});
    for (const int& dim: shape) {
        inputInfo->nodes[0].shape.push_back(dim);
    }

    for (int idx = 0; idx < interpreter->inputs().size(); ++idx) {
        TfLiteIntArray* dims = interpreter->tensor(interpreter->inputs()[idx])->dims;
        targetInfo->nodes.push_back({});
        for (int i = 0; i < dims->size; ++i) {
            if (dims->data[i] != 1) {
                targetInfo->nodes[idx].shape.push_back(dims->data[i]);
            }
        }
    }

    if(processedBuffer == nullptr) {
        processedBuffer = new uint8_t[targetInfo->nodes[0].getSize()];
        memset(processedBuffer, 0, sizeof(int8_t) * targetInfo->nodes[0].getSize());
    } else {
        return PROCESSOR_FAIL;
    }
    for (int inputIdx = 0; inputIdx < interpreter->outputs().size(); inputIdx++) {
        TfLiteIntArray* dims = interpreter->tensor(interpreter->outputs()[inputIdx])->dims;
        outputInfo.nodes.push_back({});
        for (int idx = 0; idx < dims->size; ++idx) {
            outputInfo.nodes[inputIdx].shape.push_back(dims->data[idx]);
        }
    }

    if(filter == nullptr) {
        filter = new SimpleAverageFilter<uint8_t>();
    }
    filter->setup(*inputInfo, *targetInfo);

    return PROCESSOR_SUCCESS;
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
int Processor<INTPUT_TYPE, OUTPUT_TYPE>::inference(INTPUT_TYPE* inputBuffer, OUTPUT_TYPE* output) {
    filter->process(inputBuffer, processedBuffer);

    size_t size = targetInfo->nodes[0].getSize();
    memcpy(interpreter->typed_input_tensor<INTPUT_TYPE>(0), processedBuffer, sizeof(INTPUT_TYPE) * size);

    // Run inference
    TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);

    size_t start = 0;
    std::vector<int> outputIndices = interpreter->outputs();
    for (int idx = 0; idx < outputIndices.size(); ++idx) {
        memcpy(output + start,
               interpreter->typed_tensor<OUTPUT_TYPE>(outputIndices[idx]),
               sizeof(OUTPUT_TYPE) * outputInfo.nodes[idx].getSize()
        );
        start += outputInfo.nodes[idx].getSize();
    }

    return PROCESSOR_SUCCESS;
}
////////////////////////////////////////////////////////////////////////////

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
int Processor<INTPUT_TYPE, OUTPUT_TYPE>::destroy() {
    DELETE_ARRAY(processedBuffer)
    DELETE_ARRAY(modelBuffer)
    DELETE(filter)
    DELETE(inputInfo)
    DELETE(targetInfo)
    TfLiteGpuDelegateV2Delete(delegate);
    return PROCESSOR_SUCCESS;
}