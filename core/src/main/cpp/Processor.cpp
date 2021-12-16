//
// Created by newst on 2021-12-02.
//

#include "include/Processor.h"
#include "include/SimpleAverageFilter.h"
#include "lodepng.h"

#define INPUT_INDEX 0

#define IMAGE_COUNT_INDEX 0
#define IMAGE_WIDTH_INDEX 1
#define IMAGE_HEIGHT_INDEX 2
#define IMAGE_PIXEL_STRIDE_INDEX 3


Processor::Processor():
    processedBuffer(nullptr),
    filter(nullptr),
    saveImageFlag(false), modelBuffer(nullptr) {
}
Processor::~Processor() {
    destroy();
}

int Processor::loadModel(const char* file, size_t fileSize) {
    modelBuffer = new char[fileSize];
    memcpy(modelBuffer, file, sizeof(char) * fileSize);

    model = tflite::FlatBufferModel::VerifyAndBuildFromBuffer(modelBuffer, fileSize);
    TFLITE_MINIMAL_CHECK(model != nullptr);
    builder = std::make_unique<tflite::InterpreterBuilder>(*model, resolver);
    builder->operator()(&interpreter);
    TFLITE_MINIMAL_CHECK(interpreter != nullptr);

    // Allocate tensor buffers.
    TFLITE_MINIMAL_CHECK(interpreter->AllocateTensors() == kTfLiteOk);
    printf("=== Pre-invoke Interpreter State ===\n");
    tflite::PrintInterpreterState(interpreter.get());

    return SUCCESS;
}


int Processor::setup(int width, int height, int pixelStride) {
    if(width == 0 || height == 0 || pixelStride == 0) {
        return FAIL;
    }

    ImageInfo inputInfo{
        width,
        height,
        pixelStride
    };

    TfLiteTensor* tensor = interpreter->input_tensor(INPUT_INDEX);
    if(tensor->dims->size <= IMAGE_PIXEL_STRIDE_INDEX) {
        return FAIL;
    }

    ImageInfo targetInfo{
        tensor->dims->data[IMAGE_WIDTH_INDEX],
        tensor->dims->data[IMAGE_HEIGHT_INDEX],
        tensor->dims->data[IMAGE_PIXEL_STRIDE_INDEX]
    };

    if(processedBuffer == nullptr) {
        processedBuffer = new uint8_t[targetInfo.width * targetInfo.height * targetInfo.pixelStride];
        memset(processedBuffer, 0, sizeof(int8_t) * targetInfo.width * targetInfo.height * targetInfo.pixelStride);
    } else {
        return FAIL;
    }
    if(filter == nullptr) {
        filter = new SimpleAverageFilter<uint8_t>();
    }
    filter->setup(inputInfo, targetInfo);

    return SUCCESS;
}

int Processor::inference(uint8_t* inputBuffer, float* output) {
    filter->process(inputBuffer, processedBuffer);

    if(saveImageFlag) {
        std::vector<std::uint8_t> pngBuffer(filter->getTargetInfo().height * filter->getTargetInfo().width * 4);

        for(unsigned int h = 0; h < filter->getTargetInfo().height; ++h)
        {
            for(unsigned int w = 0; w < filter->getTargetInfo().width; ++w)
            {
                for(unsigned int ps = 0; ps < filter->getTargetInfo().pixelStride; ++ps) {
                    unsigned int pos1 = (filter->getTargetInfo().width * 4) * h + 4 * w;
                    unsigned int pos2 = (filter->getTargetInfo().width * filter->getTargetInfo().pixelStride) * h + filter->getTargetInfo().pixelStride * w;
                    pngBuffer[pos1] = processedBuffer[pos2];
                    pngBuffer[pos1 + 1] = processedBuffer[pos2 + 1];
                    pngBuffer[pos1 + 2] = processedBuffer[pos2 + 2];
                    pngBuffer[pos1 + 3] = processedBuffer[pos2 + 3];

                }
            }
        }

        std::vector<std::uint8_t> imageBuffer;
        lodepng::encode(imageBuffer, pngBuffer, filter->getTargetInfo().width, filter->getTargetInfo().height);
        lodepng::save_file(imageBuffer, "/sdcard/Test/SomeImage.png");

        saveImageFlag = false;
    }

    size_t size = filter->getTargetInfo().width * filter->getTargetInfo().height * filter->getTargetInfo().pixelStride;
    memcpy(interpreter->typed_input_tensor<uint8_t>(0), processedBuffer, sizeof(uint8_t) * size);

    // Run inference
    TFLITE_MINIMAL_CHECK(interpreter->Invoke() == kTfLiteOk);

    std::vector<int> outputIndices = interpreter->outputs();
    for(int i = 0; i < outputIndices.size(); ++i) {
        output[i] = interpreter->typed_tensor<float>(outputIndices[i])[0];
    }

    return SUCCESS;
}


void Processor::saveImage() {
    saveImageFlag = true;
}

////////////////////////////////////////////////////////////////////////////

int Processor::destroy() {
    DELETE_ARRAY(processedBuffer)
    DELETE_ARRAY(modelBuffer)
    DELETE(filter)

    return SUCCESS;
}