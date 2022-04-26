//
// Created by newst on 2021-12-11.
//

#ifndef NOON_IMAGEFILTER_H
#define NOON_IMAGEFILTER_H

#include "Utils.h"
#include "BaseFilter.h"

const static int INPUT_TYPE = 0;
const static int WIDTH = 0;
const static int HEIGHT = 1;
const static int PIXER_STRID = 2;

template<typename INTPUT_TYPE>
class ImageFilter: public BaseFilter {
public:
    ImageFilter() {}
    virtual ~ImageFilter() {}

    virtual int setup(const BaseInfo& inputInfo, const BaseInfo& targetInfo) = 0;
    virtual int process(void* originalInput, void* originalOutput) = 0;

    const BaseInfo& getTargetInfo() const {
        return targetInfo;
    }
    const BaseInfo& getInputInfo() const {
        return inputInfo;
    }
protected:
    BaseInfo inputInfo;
    BaseInfo targetInfo;
};

#endif //NOON_IMAGEFILTER_H
