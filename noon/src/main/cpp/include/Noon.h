//
// Created by newst on 2022-04-16.
//

#ifndef NOON_NOON_H
#define NOON_NOON_H
#include "BasePostProcessor.h"
#include "BasePreProcessor.h"
#include "Processor.h"

enum {
    FAIL = -1,
    SUCCESS = 0,
    UNKNOWN_INFERENCE_TYPE = 1,
    PRE_PROCESSOR_ERROR = 2,
    PROCESSOR_NOT_INITIALIZED = 3,
    POST_PROCESSOR_ERROR = 4,
    BUFFER_SIZE_ZERO_ERROR = 5
};

enum class InferenceType {
    IMAGE = 0,
    AUDIO = 1
};

struct InputInfo {
    std::vector<int> shape;
};

struct OutputInfo {
    std::vector<int> shape;
};

struct InferenceInfo {
    int type;
    InputInfo input;
    OutputInfo output;
};


const char* const BM_PRE_PROCESSING = "pre";
const char* const BM_PROCESSING = "pro";
const char* const BM_POST_PROCESSING = "post";

template <typename INPUT_TYPE, typename OUTPUT_TYPE>
class Noon {
public:
    Noon();
    ~Noon();

    int setup(const InferenceInfo& info);
    int loadModel(const char* file, size_t fileSize);
    int inference(INPUT_TYPE* inputBuffer, OUTPUT_TYPE* outputBuffer);

    const std::string& getBenchmark(const std::string&);
private:
    void bypassInputData(INPUT_TYPE* inputBuffer, INPUT_TYPE* outputBuffer);
    void bypassOutputData(OUTPUT_TYPE* inputBuffer, OUTPUT_TYPE* outputBuffer);

    INPUT_TYPE* processedInputBuffer;
    OUTPUT_TYPE* processedOutputBuffer;
    size_t inputBufferSize;
    size_t outputBufferSize;

    BasePreProcessor<INPUT_TYPE, OUTPUT_TYPE>* preProcessor;
    Processor<INPUT_TYPE, OUTPUT_TYPE>* processor;
    BasePostProcessor<INPUT_TYPE, OUTPUT_TYPE>* postProcessor;

    std::unordered_map<std::string, std::string> benchmarkResults;
    InferenceType type;
};
#include "../Noon.hpp"
#endif //NOON_NOON_H
