//
// Created by newst on 2021-12-11.
//

#include "SimpleAverageFilter.h"

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
    return SUCCESS;
}

template<typename T>
int SimpleAverageFilter<T>::process(T* input, T* output) {
    return SUCCESS;
}