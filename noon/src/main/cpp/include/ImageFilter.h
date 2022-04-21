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

template <typename T>
class ImageFilter {
public:
    ImageFilter() {}
    virtual ~ImageFilter() {}

    virtual int setup(const BaseInfo& inputInfo, const BaseInfo& targetInfo) = 0;
    virtual int process(T* input, T* output) = 0;

    const BaseInfo& getTargetInfo() const {
        return targetInfo;
    }
    const BaseInfo& getInputInfo() const {
        return inputInfo;
    }
protected:
    inline T getInputValue(T* buf, int w, int h, int ps) {

        if(buf == nullptr || inputInfo.nodes[INPUT_TYPE].shape[WIDTH] == 0 || inputInfo.nodes[INPUT_TYPE].shape[HEIGHT] == 0 || inputInfo.nodes[INPUT_TYPE].shape[PIXER_STRID] == 0) {
            return 0;
        }
        return buf[inputInfo.nodes[INPUT_TYPE].shape[WIDTH] * inputInfo.nodes[INPUT_TYPE].shape[PIXER_STRID] * h + inputInfo.nodes[INPUT_TYPE].shape[PIXER_STRID] * w + ps];
    }
    inline void setTargetValue(T* buf, int w, int h, int ps, T v) {
        if(buf == nullptr || inputInfo.nodes[INPUT_TYPE].shape[WIDTH] == 0 || inputInfo.nodes[INPUT_TYPE].shape[HEIGHT] == 0 || inputInfo.nodes[INPUT_TYPE].shape[PIXER_STRID] == 0) {
            return;
        }
        buf[inputInfo.nodes[INPUT_TYPE].shape[WIDTH] * inputInfo.nodes[INPUT_TYPE].shape[PIXER_STRID] * h + inputInfo.nodes[INPUT_TYPE].shape[PIXER_STRID] * w + ps] = v;
    }

    BaseInfo inputInfo;
    BaseInfo targetInfo;
};

#endif //NOON_IMAGEFILTER_H
