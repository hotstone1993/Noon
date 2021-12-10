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
    SUCCESS = 0,
    FAIL = -1
};

struct ImageInfo {
    int width;
    int height;
    int pixelStride;
};

class Processor {
public:
    Processor();
    virtual ~Processor();

    int loadModel(const char* file, size_t fileSize);
    int inference(int8_t* inputBuffer, int size);
    int setup(int width, int height, int pixelStride);
private:
    void toProcessBuffer(int8_t* inputBuffer);
    int destroy();

    std::unique_ptr<tflite::Interpreter> interpreter;
    ImageInfo inputInfo;
    ImageInfo targetInfo;
    int8_t* processBuffer;
    float* outputBuffer;
};

#endif //NOON_PROCESSOR_H
