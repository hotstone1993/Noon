//
// Created by newst on 2022-04-16.
//
#include "include/Utils.h"
#include "include/Noon.h"

Noon::Noon(): ml(nullptr),
                mlType(TENSORFLOW_LITE),
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
    deinit();
}

void Noon::deinit() {
    deAllocBuffer(&processedInputBuffer, input);
    deAllocBuffer(&processedOutputBuffer, output);

    DELETE(preProcessor)
    DELETE(processor)
    DELETE(postProcessor)
}

NoonResult Noon::loadModel(const char* file, size_t fileSize, MLMode mlType, BaseMLInfo& info) {
    this->mlType = mlType;
    if (mlType == TENSORFLOW_LITE) {
        ml = std::make_shared<NoonTensorFlowLite>();
    } else {
        return FAIL;
    }
    ml->loadModel(file, fileSize, info);
    ml->getType(input, output);

    return SUCCESS;
}

NoonResult Noon::setup(const InferenceInfo& info) {
    NoonResult result = SUCCESS;

    if (processor == nullptr) {
        processor = new Processor(ml);
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

    if (info.output.shape.empty()) {
        outputBufferSize = 0;
        for (int idx = 0; idx < getOutputArraySize(); ++idx) {
            size_t size = getOutputBufferSize(idx);
            if (size > outputBufferSize) {
                outputBufferSize = size;
            }
        }
    } else {
        for (const auto& num: info.output.shape) {
            outputBufferSize *= num;
        }
    }

    try {
        if (inputBufferSize >= 1) {
            allocBuffer(&processedInputBuffer ,input, inputBufferSize);
        } else {
            return BUFFER_SIZE_ZERO_ERROR;
        }

        if (outputBufferSize >= 1) {
            allocBuffer(&processedOutputBuffer ,output, outputBufferSize);
        }
    } catch (std::bad_alloc& exception) {
        deinit();
        return OUT_OF_MEMORY;
    }

    return result;
}

const std::string& Noon::getBenchmark(const std::string& key) {
    return benchmarkResults[key];
}

int Noon::getInputArraySize() {
    if (mlType == TENSORFLOW_LITE) {
        if (ml == nullptr) {
            return FAIL;
        }
        return static_cast<NoonTensorFlowLite*>(ml.get())->getInputArraySize();
    } else if (mlType == PYTORCH) {
        return FAIL;
    } else {
        return FAIL;
    }
}

int Noon::getOutputArraySize() {
    if (mlType == TENSORFLOW_LITE) {
        if (ml == nullptr) {
            return FAIL;
        }
        return static_cast<NoonTensorFlowLite*>(ml.get())->getOutputArraySize();
    } else if (mlType == PYTORCH) {
        return FAIL;
    } else {
        return FAIL;
    }
}

NoonType Noon::getInputDataType(int idx) {
    if (mlType == TENSORFLOW_LITE) {
        if (ml == nullptr) {
            return NoonUnknown;
        }
        return static_cast<NoonTensorFlowLite*>(ml.get())->getInputDataType(idx);
    } else if (mlType == PYTORCH) {
        return NoonUnknown;
    } else {
        return NoonUnknown;
    }
}

NoonType Noon::getOutputDataType(int idx) {
    if (mlType == TENSORFLOW_LITE) {
        if (ml == nullptr) {
            return NoonUnknown;
        }
        return static_cast<NoonTensorFlowLite*>(ml.get())->getOutputDataType(idx);
    } else if (mlType == PYTORCH) {
        return NoonUnknown;
    } else {
        return NoonUnknown;
    }
}

size_t Noon::getInputBufferSize(int idx) {
    if (mlType == TENSORFLOW_LITE) {
        if (ml == nullptr) {
            return 0;
        }
        return static_cast<NoonTensorFlowLite*>(ml.get())->getInputBufferSize(idx);
    } else if (mlType == PYTORCH) {
        return 0;
    } else {
        return 0;
    }
}

size_t Noon::getOutputBufferSize(int idx) {
    if (mlType == TENSORFLOW_LITE) {
        if (ml == nullptr) {
            return 0;
        }
        return static_cast<NoonTensorFlowLite*>(ml.get())->getOutputBufferSize(idx);
    } else if (mlType == PYTORCH) {
        return 0;
    } else {
        return 0;
    }
}
// =================================================================================================
