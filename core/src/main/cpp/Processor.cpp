//
// Created by newst on 2021-12-02.
//

#include "include/Processor.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#define TFLITE_MINIMAL_CHECK(x)                              \
if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    return FAIL; \
}



Processor::Processor():
    width(0),
    height(0),
    pixelStride(0) {
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

    return SUCESS;
}


int Processor::setup(int width, int height, int pixelStride) {
    if(width == 0 || height == 0 || pixelStride == 0) {
        return FAIL;
    }

    this->width = width;
    this->height = height;
    this->pixelStride = pixelStride;

    return SUCESS;
}

int Processor::inference(int8_t* arr, int size) {
    const std::vector<int>&input = interpreter->inputs();

    // Run inference
    TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);
    printf("\n\n=== Post-invoke Interpreter State ===\n");
    tflite::PrintInterpreterState(interpreter.get());

    return SUCESS;
}

int Processor::destroy() {

    return SUCESS;
}