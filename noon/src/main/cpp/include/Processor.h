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

enum {
    PROCESSOR_SUCCESS = 0,
    PROCESSOR_FAIL = -1,
    NOT_INITIALIZED = -2
};

enum {
    IMAGE = 0,
    AUDIO = 1,
    UNKNOWN_TYPE = 2
};

enum {
    CPU = 0,
    GPU = 1,
    UNKNOWN_DELEGATE = 2
};

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
class Processor {
public:
    Processor();
    virtual ~Processor();

    int loadModel(const int8_t* file, size_t fileSize, int delegate);
    int inference(INTPUT_TYPE* inputBuffer, OUTPUT_TYPE* output);
    int setup(int type, const std::vector<int>& shape);

    std::vector<Node>& getNodes() {
        return outputInfo.nodes;
    }
private:
    int destroy();
    std::unique_ptr<tflite::FlatBufferModel> model;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    std::unique_ptr<tflite::InterpreterBuilder> builder;
    TfLiteDelegate* delegate;

    char* modelBuffer;
    INTPUT_TYPE* processedBuffer;
    BaseInfo outputInfo;
    ImageFilter<INTPUT_TYPE>* filter;

    BaseInfo* inputInfo;
    BaseInfo* targetInfo;
};
#include "../Processor.hpp"
#endif //NOON_PROCESSOR_H
