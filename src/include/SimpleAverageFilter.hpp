//
// Created by newst on 2021-12-11.
//
#include <future>

template<typename T>
void subProcess(T* input, T* output, unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, BaseInfo inputInfo, BaseInfo targetInfo) {
    T A, B, C, D, pixel;
    int currX, currY;
    int inputWidth = inputInfo.nodes[INPUT_TYPE].shape[WIDTH];
    int inputHeight = inputInfo.nodes[INPUT_TYPE].shape[HEIGHT];
    int inputPixelStride = inputInfo.nodes[INPUT_TYPE].shape[PIXER_STRID];
    int targetWidth = targetInfo.nodes[INPUT_TYPE].shape[WIDTH];
    int targetHeight = targetInfo.nodes[INPUT_TYPE].shape[HEIGHT];
    int targetPixelStride = targetInfo.nodes[INPUT_TYPE].shape[PIXER_STRID];

    float xRatio = ((float)(inputWidth - 1)) / targetWidth;
    float yRatio = ((float)(inputHeight - 1)) / targetHeight;
    float xDiff, yDiff;

    for(unsigned int x = startX; x < endX; ++x) {
        for(unsigned int y = startY; y < endY; ++y) {
            for(unsigned int ps = 0; ps < targetPixelStride; ++ps) {
                currX = static_cast<int>(xRatio * x);
                currY = static_cast<int>(yRatio * y);
                xDiff = (xRatio * x) - currX;
                yDiff = (yRatio * y) - currY;

                A = input[inputWidth * inputPixelStride * currY + inputPixelStride * currX + ps];
                B = input[inputWidth * inputPixelStride * currY + inputPixelStride * (currX + 1) + ps];
                C = input[inputWidth * inputPixelStride * (currY + 1) + inputPixelStride * currX + ps];
                D = input[inputWidth * inputPixelStride * (currY + 1) + inputPixelStride * (currX + 1) + ps];
                pixel = static_cast<T>(A * (1 - xDiff) * (1 - yDiff) + B * (xDiff) * (1 - yDiff) + C * (yDiff) * (1 - xDiff) + D * (xDiff * yDiff));
                output[targetWidth * targetPixelStride * x + targetPixelStride * (targetHeight - y - 1) + ps] = pixel;
            }
        }
    }
}

template<typename T>
SimpleAverageFilter<T>::SimpleAverageFilter() {
}

template<typename T>
SimpleAverageFilter<T>::~SimpleAverageFilter() {
}

template<typename T>
int SimpleAverageFilter<T>::setup(const BaseInfo& inputInfo, const BaseInfo& targetInfo) {
    this->inputInfo = inputInfo;
    this->targetInfo = targetInfo;
    calculateKernelAndStep();
    return 0;
}

template<typename T>
int SimpleAverageFilter<T>::process(void* originalInput, void* originalOutput) {
    T* input = static_cast<T*>(originalInput);
    T* output = static_cast<T*>(originalOutput);
    //subProcess<T>(input, output, 0, 0, this->targetInfo.width, this->targetInfo.height, this->inputInfo, this->targetInfo);
    auto f1 = std::async(std::launch::async, subProcess<T>, input, output, 0, 0, this->targetInfo.nodes[INPUT_TYPE].shape[WIDTH] / 2, this->targetInfo.nodes[INPUT_TYPE].shape[HEIGHT] / 2, this->inputInfo, this->targetInfo);
    auto f2 = std::async(std::launch::async, subProcess<T>, input, output, this->targetInfo.nodes[INPUT_TYPE].shape[WIDTH] / 2, 0, this->targetInfo.nodes[INPUT_TYPE].shape[WIDTH], this->targetInfo.nodes[INPUT_TYPE].shape[HEIGHT] / 2, this->inputInfo, this->targetInfo);
    auto f3 = std::async(std::launch::async, subProcess<T>, input, output, 0, this->targetInfo.nodes[INPUT_TYPE].shape[HEIGHT] / 2, this->targetInfo.nodes[INPUT_TYPE].shape[WIDTH] / 2, this->targetInfo.nodes[INPUT_TYPE].shape[HEIGHT], this->inputInfo, this->targetInfo);
    auto f4 = std::async(std::launch::async, subProcess<T>, input, output, this->targetInfo.nodes[INPUT_TYPE].shape[WIDTH] / 2, this->targetInfo.nodes[INPUT_TYPE].shape[HEIGHT] / 2, this->targetInfo.nodes[INPUT_TYPE].shape[WIDTH], this->targetInfo.nodes[INPUT_TYPE].shape[HEIGHT], this->inputInfo, this->targetInfo);

    f1.get();
    f2.get();
    f3.get();
    f4.get();

    return 0;
}

template<typename T>
void SimpleAverageFilter<T>::calculateKernelAndStep() {
    kernel.width = 1;
    kernel.height = 1;
    stepX= 1;
    stepY = 1;
}