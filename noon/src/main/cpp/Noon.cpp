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

    if (inputBufferSize == 1 || outputBufferSize == 1) {
        return BUFFER_SIZE_ZERO_ERROR;
    }

    allocBuffer(&processedInputBuffer ,input, inputBufferSize);
    allocBuffer(&processedOutputBuffer ,output, outputBufferSize);

    return result;
}

NoonResult Noon::inference(void* inputBuffer, void* outputBuffer) {
    NoonResult result = SUCCESS;
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    auto* typedInputBuffer = (uint8_t *) inputBuffer;
    auto* typedProcessedInputBuffer = (uint8_t *) processedInputBuffer;
    auto* typedOutputBuffer = (float *) outputBuffer;

    if (preProcessor != nullptr) {
        result = static_cast<NoonResult>(preProcessor->inference(typedInputBuffer,
                                                                 typedProcessedInputBuffer));
    } else {
        bypassInputData<uint8_t>(&typedInputBuffer, &typedProcessedInputBuffer);
    }
    if (result != SUCCESS) {
        return result;
    }

    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
    benchmarkResults[BM_PRE_PROCESSING] = std::to_string(duration.count()) + "ms";
    start = std::chrono::system_clock::now();

    if (processor != nullptr) {
        result = static_cast<NoonResult>(processor->inference<uint8_t, float>(typedProcessedInputBuffer, typedOutputBuffer));
        if (result != SUCCESS) {
            return result;
        }
    } else {
        return PROCESSOR_NOT_INITIALIZED;
    }

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
    benchmarkResults[BM_PROCESSING] = std::to_string(duration.count()) + "ms";
//    start = std::chrono::system_clock::now();
//
//    if (postProcessor != nullptr) {
//        result = postProcessor->inference(processedOutputBuffer, typeOutputBuffer);
//    } else {
//        setZeroToOutputData(typeOutputBuffer);
//        result = NOT_PROCESSED;
//    }
//
//    duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
//    benchmarkResults[BM_POST_PROCESSING] = std::to_string(duration.count()) + "ms";

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