//
// Created by newst on 2022-04-16.
//
#include "include/Utils.h"
#include "include/Noon.h"
#include <chrono>

Noon::Noon(): ml(nullptr),
                processedInputBuffer(nullptr),
                processedOutputBuffer(nullptr),
                inputBufferSize(1),
                outputBufferSize(1),
                preProcessor(nullptr),
                processor(nullptr),
                postProcessor(nullptr),
                inferenceType(IMAGE) {
}

Noon::~Noon() {
    DELETE(processedInputBuffer)
    DELETE(processedOutputBuffer)

    DELETE(preProcessor)
    DELETE(processor)
    DELETE(postProcessor)
}

NoonResult Noon::loadModel(const char* file, size_t fileSize, MLMode mlType) {
    if (mlType == TENSORFLOW_LITE) {
        ml = new NoonTensorFlowLite();
    } else {
        return FAIL;
    }
    ml->loadModel(file, fileSize, mlType);
    ml->getType(input, output);

    return SUCCESS;
}

NoonResult Noon::setup(const InferenceInfo& info) {
    NoonResult result = SUCCESS;

    if (info.delegate >= UNKNOWN_DELEGATE) {
        return UNKNOWN_INFERENCE_DELEGATE;
    }

    if (info.model != nullptr) {
        if (processor == nullptr) {
            processor = new Processor(ml);
        }
    } else {
        return PROCESSOR_NOT_INITIALIZED;
    }

    if (info.type >= UNKNOWN_TYPE) {
        return UNKNOWN_INFERENCE_TYPE;
    } else {
        inferenceType = info.type;
    }

    result = static_cast<NoonResult>(processor->setup(info.type, input, info.input.shape));
    if (result != SUCCESS) {
        return result;
    }

    for (const auto& num: info.input.shape) {
        inputBufferSize *= num;
    }
    for (const auto& num: info.output.shape) {
        outputBufferSize *= num;
    }

    if (inputBufferSize > 1) {
        allocBuffer(&processedInputBuffer ,input, inputBufferSize);
    } else {
        return BUFFER_SIZE_ZERO_ERROR;
    }

    if (outputBufferSize > 1) {
        allocBuffer(&processedOutputBuffer ,output, outputBufferSize);
    }

    return result;
}

const std::string& Noon::getBenchmark(const std::string& key) {
    return benchmarkResults[key];
}

// =================================================================================================

void Noon::setZeroToOutputData(void** outputBuffer) {
    for (int idx = 0; idx < processor->getNodes().size(); ++idx) {
        memset(outputBuffer[idx], 0, sizeof(getNoonTypeSize(output)) * processor->getNodes()[idx].getSize());
    }
}