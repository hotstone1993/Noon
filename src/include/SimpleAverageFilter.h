//
// Created by newst on 2021-12-11.
//

#ifndef NOON_SIMPLEAVERAGEFILTER_H
#define NOON_SIMPLEAVERAGEFILTER_H

#include "ImageFilter.h"

template<typename INTPUT_TYPE>
class SimpleAverageFilter: public ImageFilter<INTPUT_TYPE> {
public:
    SimpleAverageFilter();
    virtual ~SimpleAverageFilter();

    virtual int setup(const BaseInfo& inputInfo, const BaseInfo& targetInfo) override;
    virtual int process(void* originalInput, void* originalOutput) override;

private:
    void calculateKernelAndStep();

    Rectrangle kernel;
    unsigned int stepX;
    unsigned int stepY;
};
#include "SimpleAverageFilter.hpp"
#endif //NOON_SIMPLEAVERAGEFILTER_H
