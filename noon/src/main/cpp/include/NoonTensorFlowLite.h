//
// Created by newst on 2022-04-26.
//

#ifndef NOON_NOONTENSORFLOWLITE_H
#define NOON_NOONTENSORFLOWLITE_H
#include "BaseML.h"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/optional_debug_tools.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"

class NoonTensorFlowLite: public BaseML {
public:
    NoonTensorFlowLite();
    ~NoonTensorFlowLite();

    int loadModel(const char* file, size_t fileSize, int delegate, int numThread);
    void getType(NoonType& input, NoonType& output);

    tflite::Interpreter* getInterpreter() {
        return interpreter.get();
    }
private:
    std::unique_ptr<tflite::FlatBufferModel> model;
    tflite::ops::builtin::BuiltinOpResolver resolver;
    std::unique_ptr<tflite::Interpreter> interpreter;
    std::unique_ptr<tflite::InterpreterBuilder> builder;
    TfLiteDelegate* delegate;

    char* modelBuffer;
};

#endif //NOON_NOONTENSORFLOWLITE_H
