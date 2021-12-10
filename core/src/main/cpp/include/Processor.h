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

enum {
    SUCESS = 0,
    FAIL = -1
};

class Processor {
public:
    Processor();
    virtual ~Processor();

    int loadModel(const char* file, size_t fileSize);
    int inference(int8_t* arr, int size);
    int setup(int width, int height, int pixelStride);
private:
    int destroy();

    std::unique_ptr<tflite::Interpreter> interpreter;
    int width;
    int height;
    int pixelStride;
};

#endif //NOON_PROCESSOR_H
