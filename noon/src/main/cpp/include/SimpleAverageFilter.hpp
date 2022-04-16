//
// Created by newst on 2021-12-11.
//
#include <future>

template<typename T>
void subProcess(T* input, T* output, unsigned int startX, unsigned int startY, unsigned int endX, unsigned int endY, ImageInfo inputInfo, ImageInfo targetInfo) {
    T A, B, C, D, pixel;
    int currX, currY;
    float xRatio = ((float)(inputInfo.width - 1)) / targetInfo.width;
    float yRatio = ((float)(inputInfo.height - 1)) / targetInfo.height;
    float xDiff, yDiff;

    for(unsigned int x = startX; x < endX; ++x) {
        for(unsigned int y = startY; y < endY; ++y) {
            for(unsigned int ps = 0; ps < targetInfo.pixelStride; ++ps) {
                currX = static_cast<int>(xRatio * x);
                currY = static_cast<int>(yRatio * y);
                xDiff = (xRatio * x) - currX;
                yDiff = (yRatio * y) - currY;

                A = input[inputInfo.width * inputInfo.pixelStride * currY + inputInfo.pixelStride * currX + ps] & 0xff ;
                B = input[inputInfo.width * inputInfo.pixelStride * currY + inputInfo.pixelStride * (currX + 1) + ps] & 0xff ;
                C = input[inputInfo.width * inputInfo.pixelStride * (currY + 1) + inputInfo.pixelStride * currX + ps] & 0xff ;
                D = input[inputInfo.width * inputInfo.pixelStride * (currY + 1) + inputInfo.pixelStride * (currX + 1) + ps] & 0xff ;
                pixel = static_cast<T>(A * (1 - xDiff) * (1 - yDiff) + B * (xDiff) * (1 - yDiff) + C * (yDiff) * (1 - xDiff) + D * (xDiff * yDiff));
                output[targetInfo.width * targetInfo.pixelStride * x + targetInfo.pixelStride * (targetInfo.height - y - 1) + ps] = pixel;
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
int SimpleAverageFilter<T>::setup(ImageInfo inputInfo, ImageInfo targetInfo) {
    this->inputInfo = inputInfo;
    this->targetInfo = targetInfo;
    calculateKernelAndStep();
    return 0;
}

template<typename T>
int SimpleAverageFilter<T>::process(T* input, T* output) {
    //subProcess<T>(input, output, 0, 0, this->targetInfo.width, this->targetInfo.height, this->inputInfo, this->targetInfo);
    auto f1 = std::async(std::launch::async, subProcess<T>, input, output, 0, 0, this->targetInfo.width / 2, this->targetInfo.height / 2, this->inputInfo, this->targetInfo);
    auto f2 = std::async(std::launch::async, subProcess<T>, input, output, this->targetInfo.width / 2, 0, this->targetInfo.width, this->targetInfo.height / 2, this->inputInfo, this->targetInfo);
    auto f3 = std::async(std::launch::async, subProcess<T>, input, output, 0, this->targetInfo.height / 2, this->targetInfo.width / 2, this->targetInfo.height, this->inputInfo, this->targetInfo);
    auto f4 = std::async(std::launch::async, subProcess<T>, input, output, this->targetInfo.width / 2, this->targetInfo.height / 2, this->targetInfo.width, this->targetInfo.height, this->inputInfo, this->targetInfo);

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