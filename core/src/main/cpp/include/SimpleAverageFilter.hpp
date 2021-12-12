//
// Created by newst on 2021-12-11.
//

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
    return SUCCESS;
}

template<typename T>
int SimpleAverageFilter<T>::process(T* input, T* output) {
    unsigned int oh = 0;
    unsigned int ow = 0;
    for(unsigned int h = 0; h < this->inputInfo.height && oh < this->targetInfo.height; h += stepY, ++oh) {
        ow = 0;
        for(unsigned int w = 0; w < this->inputInfo.width && ow < this->targetInfo.width; w += stepX, ++ow) {
            for(unsigned int ps = 0; ps < this->targetInfo.pixelStride; ++ps) {
                T sum = 0;

                for(unsigned int kh = h; kh < h + kernel.height; ++kh) {
                    for(unsigned int kw = w; kw < w + kernel.height; ++kw) {
                        sum += this->getInputValue(input, kw, kh, ps);
                    }
                }

                this->setTargetValue(output, ow, oh, ps, sum / (kernel.height * kernel.height));
            }
        }
    }
    return SUCCESS;
}

template<typename T>

void SimpleAverageFilter<T>::calculateKernelAndStep() {
    kernel.width = 2;
    kernel.height = 1;
    stepX= 2;
    stepY = 1;
}