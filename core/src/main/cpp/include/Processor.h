//
// Created by newst on 2021-12-02.
//

#ifndef NOON_PROCESSOR_H
#define NOON_PROCESSOR_H

#include <string>

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"
#include "ImageFilter.h"

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
class Processor {
public:
    Processor();
    virtual ~Processor();

    int loadModel(const char* file, size_t fileSize);
    int inference(INTPUT_TYPE* inputBuffer, OUTPUT_TYPE* output);
    int setup(int width, int height, int pixelStride);
    void saveImage();
    const std::string& getBenchmark();
private:
    int destroy();
    std::unique_ptr<tflite::FlatBufferModel> model;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    std::unique_ptr<tflite::InterpreterBuilder> builder;
    TfLiteDelegate* delegate;

    char* modelBuffer;
    bool saveImageFlag;
    INTPUT_TYPE* processedBuffer;
    ImageFilter<uint8_t>* filter;
    std::string benchmarkResult;
};
#include "../Processor.hpp"
#endif //NOON_PROCESSOR_H
