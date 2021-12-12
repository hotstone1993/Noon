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
#include "ImageFilter.h"

class Processor {
public:
    Processor();
    virtual ~Processor();

    int loadModel(const char* file, size_t fileSize);
    int inference(uint8_t* inputBuffer, float* output);
    int setup(int width, int height, int pixelStride);
    void saveImage();
private:
    int destroy();
    std::unique_ptr<tflite::FlatBufferModel> model;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    std::unique_ptr<tflite::InterpreterBuilder> builder;

    bool saveImageFlag;
    uint8_t* processedBuffer;
    ImageFilter<uint8_t>* filter;
};

#endif //NOON_PROCESSOR_H
