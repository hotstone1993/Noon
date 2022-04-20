//
// Created by newst on 2021-12-11.
//

#ifndef NOON_SIMPLEAVERAGEFILTER_H
#define NOON_SINCFILTER_H

#include "ImageFilter.h"

template <typename T>
class SimpleAverageFilter: public ImageFilter<T> {
public:
    SimpleAverageFilter();
    virtual ~SimpleAverageFilter();

    virtual int setup(const BaseInfo& inputInfo, const BaseInfo& targetInfo) override;
    virtual int process(T* input, T* output) override;

private:
    void calculateKernelAndStep();

    Rectrangle kernel;
    unsigned int stepX;
    unsigned int stepY;
};
#include "SimpleAverageFilter.hpp"
#endif //NOON_SIMPLEAVERAGEFILTER_H
