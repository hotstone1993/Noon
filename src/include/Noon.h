//
// Created by newst on 2022-04-16.
//

#ifndef NOON_NOON_H
#define NOON_NOON_H

#include "BasePostProcessor.h"
#include "BasePreProcessor.h"
#include "Processor.h"

struct InputInfo {
    std::vector<int> shape;
};

struct OutputInfo {
    std::vector<int> shape;
};

struct InferenceInfo {
    int type;
    int8_t * model = nullptr;
    unsigned modelSize = 0;
    InputInfo input;
    OutputInfo output;
};

const char* const BM_PRE_PROCESSING = "pre";
const char* const BM_PROCESSING = "pro";
const char* const BM_POST_PROCESSING = "post";

typedef enum {
    FAIL = -1,
    SUCCESS = 0,
    UNKNOWN_INFERENCE_TYPE = 1,
    UNKNOWN_INFERENCE_DELEGATE = 2,
    PRE_PROCESSOR_ERROR = 3,
    PROCESSOR_NOT_INITIALIZED = 4,
    POST_PROCESSOR_ERROR = 5,
    BUFFER_SIZE_ZERO_ERROR = 6,
    NOT_PROCESSED = 7
} NoonResult;

typedef enum {
    TENSORFLOW_LITE = 0,
    PYTORCH = 1
} MLMode;

class Noon {
public:
    Noon();
    ~Noon();

    NoonResult loadModel(const char* file, size_t fileSize, MLMode mlType, BaseMLInfo& info);
    NoonResult setup(const InferenceInfo& info);

    template<typename INPUT_TYPE>
    NoonResult inference(INPUT_TYPE* inputBuffer) {
        NoonResult result = SUCCESS;
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        auto* typedInputBuffer = (INPUT_TYPE *) inputBuffer;
        auto* typedProcessedInputBuffer = (INPUT_TYPE *) processedInputBuffer;

        if (preProcessor != nullptr) {
            result = static_cast<NoonResult>(preProcessor->inference(typedInputBuffer,
                                                                     typedProcessedInputBuffer));
        } else {
            bypassInputData<INPUT_TYPE>(&typedInputBuffer, &typedProcessedInputBuffer);
        }
        if (result != SUCCESS) {
            return result;
        }

        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
        benchmarkResults[BM_PRE_PROCESSING] = std::to_string(duration.count()) + "ms";
        start = std::chrono::system_clock::now();

        if (processor != nullptr) {
            result = static_cast<NoonResult>(processor->inference<INPUT_TYPE>(typedProcessedInputBuffer));
            if (result != SUCCESS) {
                return result;
            }
        } else {
            return PROCESSOR_NOT_INITIALIZED;
        }

        duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
        benchmarkResults[BM_PROCESSING] = std::to_string(duration.count()) + "ms";

        return result;
    }

    template<typename OUTPUT_TYPE>
    NoonResult getOutput(int idx, OUTPUT_TYPE* outputBuffer) {
        auto* typedProcessedInputBuffer = (OUTPUT_TYPE *) processedOutputBuffer;
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        NoonResult result;

        if (processor != nullptr) {
            result = static_cast<NoonResult>(processor->getOutput<OUTPUT_TYPE>(idx, typedProcessedInputBuffer));
            if (result != SUCCESS) {
                return result;
            }
        } else {
            return PROCESSOR_NOT_INITIALIZED;
        }
        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
        benchmarkResults[BM_PROCESSING] = std::to_string(duration.count()) + "ms";
        start = std::chrono::system_clock::now();

        auto* typedProcessedOutputBuffer = (OUTPUT_TYPE *) outputBuffer;
        if (postProcessor != nullptr) {
            result = static_cast<NoonResult>(postProcessor->inference(typedProcessedInputBuffer, typedProcessedOutputBuffer));
        } else {
            bypassOutputData(&typedProcessedInputBuffer, &typedProcessedOutputBuffer, getOutputBufferSize(idx));
            result = SUCCESS;
        }

        duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
        benchmarkResults[BM_POST_PROCESSING] = std::to_string(duration.count()) + "ms";
        return result;
    }

    const std::string& getBenchmark(const std::string&);
    int getInputArraySize();
    int getOutputArraySize();
    NoonType getInputDataType(int idx);
    NoonType getOutputDataType(int idx);
    size_t getInputBufferSize(int idx);
    size_t getOutputBufferSize(int idx);
private:
    template <typename INPUT_TYPE>
    void bypassInputData(INPUT_TYPE** inputBuffer, INPUT_TYPE** outputBuffer) {
        memcpy(*outputBuffer, *inputBuffer, sizeof(INPUT_TYPE) * inputBufferSize);
    }
    template <typename OUTPUT_TYPE>
    void bypassOutputData(OUTPUT_TYPE** inputBuffer, OUTPUT_TYPE** outputBuffer, size_t size) {
        memcpy(*outputBuffer, *inputBuffer, sizeof(OUTPUT_TYPE) * size);
    }

    BaseML* ml;
    MLMode mlType;
    void* processedInputBuffer;
    void* processedOutputBuffer;
    size_t inputBufferSize;
    size_t outputBufferSize;

    BasePreProcessor* preProcessor;
    Processor* processor;
    BasePostProcessor* postProcessor;

    std::unordered_map<std::string, std::string> benchmarkResults;
    int inferenceType;
    NoonType input;
    NoonType output;
};
#endif //NOON_NOON_H
