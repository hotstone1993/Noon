//
// Created by newst on 2021-12-02.
//

#include "include/Processor.h"
#include "include/SimpleAverageFilter.h"
#if(Android)
#include <android/log.h>
#endif
#include <chrono>

Processor::Processor(std::shared_ptr<BaseML> ml):
    ml(ml),
    noonType(NoonUnknown),
    processedBuffer(nullptr),
    filter(nullptr),
    inputInfo(nullptr),
    targetInfo(nullptr) {
}

Processor::~Processor() {
    destroy();
}

int Processor::setup(int inferenceType, NoonType noonType, const std::vector<int>& shape) {
    inputInfo = new BaseInfo();
    targetInfo = new BaseInfo();

    if (inputInfo == nullptr || targetInfo == nullptr) {
        return NOT_INITIALIZED;
    }

    this->noonType = noonType;
    inputInfo->nodes.emplace_back();
    for (const int& dim: shape) {
        inputInfo->nodes[0].shape.push_back(dim);
    }
    tflite::Interpreter* interpreter = static_cast<NoonTensorFlowLite*>(ml.get())->getInterpreter();

    for (int idx = 0; idx < interpreter->inputs().size(); ++idx) {
        TfLiteIntArray* dims = interpreter->tensor(interpreter->inputs()[idx])->dims;
        targetInfo->nodes.emplace_back();
        for (int i = 0; i < dims->size; ++i) {
            if (dims->data[i] == 1)
                continue;
            targetInfo->nodes[idx].shape.push_back(dims->data[i]);
        }
    }

    if(processedBuffer == nullptr) {
        allocBuffer(&processedBuffer, noonType, getNoonTypeSize(noonType) * targetInfo->nodes[0].getSize());
        memset(processedBuffer, 0, getNoonTypeSize(noonType) * targetInfo->nodes[0].getSize());
    } else {
        return PROCESSOR_FAIL;
    }
    for (int inputIdx = 0; inputIdx < interpreter->outputs().size(); inputIdx++) {
        TfLiteIntArray* dims = interpreter->tensor(interpreter->outputs()[inputIdx])->dims;
        outputInfo.nodes.emplace_back();
        for (int idx = 0; idx < dims->size; ++idx) {
            outputInfo.nodes[inputIdx].shape.push_back(dims->data[idx]);
        }
    }

    if(filter == nullptr) {
        if (inferenceType == IMAGE) {
            ALLOC_TEMPLATE_CLASS(filter, SimpleAverageFilter, noonType)
        } else if (inferenceType == AUDIO) {
        }
    }
    filter->setup(*inputInfo, *targetInfo);

    return PROCESSOR_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////

int Processor::destroy() {
    deAllocBuffer(&processedBuffer, noonType);
    DELETE(inputInfo)
    DELETE(targetInfo)
    return PROCESSOR_SUCCESS;
}