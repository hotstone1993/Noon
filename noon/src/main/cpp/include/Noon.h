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
    int delegate;
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

    NoonResult loadModel(const char* file, size_t fileSize, MLMode mlType);
    NoonResult setup(const InferenceInfo& info);

    NoonResult inference(void* inputBuffer, void* outputBuffer);

    const std::string& getBenchmark(const std::string&);
private:
    template <typename INPUT_TYPE>
    void bypassInputData(INPUT_TYPE** inputBuffer, INPUT_TYPE** outputBuffer) {
        memcpy(*outputBuffer, *inputBuffer, sizeof(INPUT_TYPE) * inputBufferSize);
    }
    void setZeroToOutputData(void** outputBuffer);

    BaseML* ml;
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
