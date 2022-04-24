//
// Created by newst on 2022-04-16.
//
#include "include/Noon.h"
#include "Utils.h"
#include <chrono>

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
Noon<INPUT_TYPE, OUTPUT_TYPE>::Noon(): processedInputBuffer(nullptr),
                                        processedOutputBuffer(nullptr),
                                        inputBufferSize(1),
                                        outputBufferSize(1),
                                        preProcessor(nullptr),
                                        processor(nullptr),
                                        postProcessor(nullptr),
                                        type(IMAGE) {
    if (processor == nullptr) {
        processor = new Processor<INPUT_TYPE, OUTPUT_TYPE>();
    }
}

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
Noon<INPUT_TYPE, OUTPUT_TYPE>::~Noon() {
    DELETE(processedInputBuffer)
    DELETE(processedOutputBuffer)

    DELETE(preProcessor)
    DELETE(processor)
    DELETE(postProcessor)
}

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
int Noon<INPUT_TYPE, OUTPUT_TYPE>::setup(const InferenceInfo& info) {
    int result = SUCCESS;

    if (info.delegate >= UNKNOWN_DELEGATE) {
        return UNKNOWN_INFERENCE_DELEGATE;
    }

    if (info.model != nullptr) {
        if (processor != nullptr) {
            result = processor->loadModel(info.model, info.modelSize, info.delegate);
            if (result != SUCCESS) {
                return result;
            }
        } else {
            return PROCESSOR_NOT_INITIALIZED;
        }
    }

    if (info.type >= UNKNOWN_TYPE) {
        return UNKNOWN_INFERENCE_TYPE;
    } else {
        type = info.type;
    }

    result = processor->setup(info.type, info.input.shape);
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

    ALLOC_BUFFER(processedInputBuffer, INPUT_TYPE, inputBufferSize)
    ALLOC_BUFFER(processedOutputBuffer, OUTPUT_TYPE, outputBufferSize)

    return result;
}

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
int Noon<INPUT_TYPE, OUTPUT_TYPE>::inference(void* inputBuffer, void* outputBuffer) {
    int result = SUCCESS;
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    INPUT_TYPE* typeInputBuffer = reinterpret_cast<INPUT_TYPE*>(inputBuffer);
    OUTPUT_TYPE* typeOutputBuffer = reinterpret_cast<OUTPUT_TYPE*>(outputBuffer);

    if (preProcessor != nullptr) {
        result = preProcessor->inference(typeInputBuffer, processedInputBuffer);
    } else {
        bypassInputData(typeInputBuffer, processedInputBuffer);
    }
    if (result != SUCCESS) {
        return result;
    }

    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
    benchmarkResults[BM_PRE_PROCESSING] = std::to_string(duration.count()) + "ms";
    start = std::chrono::system_clock::now();

    if (processor != nullptr) {
        result = processor->inference(processedInputBuffer, typeOutputBuffer);
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

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
void Noon<INPUT_TYPE, OUTPUT_TYPE>::bypassInputData(INPUT_TYPE* inputBuffer, INPUT_TYPE* outputBuffer) {
    memcpy(outputBuffer, inputBuffer, sizeof(INPUT_TYPE) * inputBufferSize);
}

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
void Noon<INPUT_TYPE, OUTPUT_TYPE>::setZeroToOutputData(OUTPUT_TYPE** outputBuffer) {
    for (int idx = 0; idx < processor->getNodes().size(); ++idx) {
        memset(outputBuffer[idx], sizeof(OUTPUT_TYPE) * processor->getNodes().getSize());
    }
}

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
const std::string& Noon<INTPUT_TYPE, OUTPUT_TYPE>::getBenchmark(const std::string& key) {
    return benchmarkResults[key];
}

// =================================================================================================

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
int Noon<INPUT_TYPE, OUTPUT_TYPE>::loadModel(const char* file, size_t fileSize) {
    if (processor != nullptr) {
        processor->loadModel(file, fileSize);
    } else {
        return PROCESSOR_NOT_INITIALIZED;
    }

    return SUCCESS;
}
