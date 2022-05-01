//
// Created by newst on 2022-04-26.
//
#include "include/Utils.h"
#include "include/NoonTensorFlowLite.h"
#include <android/log.h>

const char* const TAG = "Noon";

#define TFLITE_MINIMAL_CHECK(x)                              \
if (!(x)) { \
    __android_log_print(ANDROID_LOG_ERROR, TAG, "Error at %s:%d\n", __FILE__, __LINE__);                                 \
    return -1; \
}

NoonType mappingType(TfLiteType type) {
    switch (type) {
        case kTfLiteFloat64:
            return NoonFloat64;
        case kTfLiteFloat32:
            return NoonFloat32;
        case kTfLiteInt64:
            return NoonInt64;
        case kTfLiteInt32:
            return NoonInt32;
        case kTfLiteUInt32:
            return NoonUInt32;
        case kTfLiteInt16:
            return NoonInt16;
        case kTfLiteInt8:
            return NoonInt8;
        case kTfLiteUInt8:
            return NoonUInt8;
        default:
            return NoonUnknown;
    }
}

NoonTensorFlowLite::NoonTensorFlowLite(): modelBuffer(nullptr),
                                          delegate(nullptr) {

}

NoonTensorFlowLite::~NoonTensorFlowLite() {
    DELETE_ARRAY(modelBuffer)
}

int NoonTensorFlowLite::loadModel(const char* file, size_t fileSize, int delegate, int numThread) {
    modelBuffer = new char[fileSize];
    memcpy(modelBuffer, file, sizeof(char) * fileSize);

    model = tflite::FlatBufferModel::VerifyAndBuildFromBuffer(modelBuffer, fileSize);
    TFLITE_MINIMAL_CHECK(model != nullptr);
    builder = std::make_unique<tflite::InterpreterBuilder>(*model, resolver);
    builder->SetNumThreads(numThread);
    builder->operator()(&interpreter);
    TFLITE_MINIMAL_CHECK(interpreter != nullptr);

    if (delegate == CPU) {
        TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
    }
    else if (delegate == GPU) {
        TfLiteGpuDelegateOptionsV2 option = TfLiteGpuDelegateOptionsV2Default();
        this->delegate = TfLiteGpuDelegateV2Create(&option);
        TFLITE_MINIMAL_CHECK(interpreter->ModifyGraphWithDelegate(this->delegate) == kTfLiteOk);
    }
    printf("=== Pre-invoke Interpreter State ===\n");
    tflite::PrintInterpreterState(interpreter.get());

    return 0;
}

void NoonTensorFlowLite::getType(NoonType& input, NoonType& output) {
    input = mappingType(interpreter->tensor(interpreter->inputs()[0])->type);
    output = mappingType(interpreter->tensor(interpreter->outputs()[0])->type);
}

