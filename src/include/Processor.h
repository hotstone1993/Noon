//
// Created by newst on 2021-12-02.
//

#ifndef NOON_PROCESSOR_H
#define NOON_PROCESSOR_H

#include <string>
#include "BaseFilter.h"
#include "NoonTensorFlowLite.h"

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

class Processor {
public:
    Processor(std::shared_ptr<BaseML> ml);
    virtual ~Processor();

    template<typename INTPUT_TYPE>
    int inference(INTPUT_TYPE* inputBuffer) {
        tflite::Interpreter* interpreter = static_cast<NoonTensorFlowLite*>(ml.get())->getInterpreter();
        filter->process(inputBuffer, processedBuffer);

        size_t size = targetInfo->nodes[0].getSize();
        memcpy(interpreter->typed_input_tensor<INTPUT_TYPE>(0), processedBuffer, sizeof(INTPUT_TYPE) * size);

        // Run inference
        if (interpreter->Invoke() != kTfLiteOk) {
            return PROCESSOR_FAIL;
        }

        return PROCESSOR_SUCCESS;
    }
    template<typename OUTPUT_TYPE>
    int getOutput(int idx, OUTPUT_TYPE* output) {
        tflite::Interpreter* interpreter = static_cast<NoonTensorFlowLite*>(ml.get())->getInterpreter();
        memcpy(output,
               interpreter->typed_tensor<OUTPUT_TYPE>(interpreter->outputs()[idx]),
               sizeof(OUTPUT_TYPE) * outputInfo.nodes[idx].getSize());

        return PROCESSOR_SUCCESS;
    }
    int setup(int inferenceType, NoonType noonType, const std::vector<int>& shape);

    std::vector<Node>& getNodes() {
        return outputInfo.nodes;
    }
private:
    int destroy();
    NoonType noonType;
    void* processedBuffer;
    BaseInfo outputInfo;
    BaseFilter* filter;

    std::shared_ptr<BaseML> ml;
    BaseInfo* inputInfo;
    BaseInfo* targetInfo;
};
#endif //NOON_PROCESSOR_H
