//
// Created by newst on 2021-12-11.
//

#ifndef NOON_IMAGEFILTER_H
#define NOON_IMAGEFILTER_H

#include "Utils.h"

struct ImageInfo {
    int width;
    int height;
    int pixelStride;
};

template <typename T>
class ImageFilter {
public:
    ImageFilter() {}
    virtual ~ImageFilter() {}

    virtual int setup(ImageInfo inputInfo, ImageInfo targetInfo) = 0;
    virtual int process(T* input, T* output) = 0;

    const ImageInfo& getTargetInfo() const {
        return targetInfo;
    }
    const ImageInfo& getInputInfo() const {
        return inputInfo;
    }
protected:
    inline T getInputValue(T* buf, int w, int h, int ps) {
        if(buf == nullptr || inputInfo.width == 0 || inputInfo.height == 0 || inputInfo.pixelStride == 0) {
            return 0;
        }
        return buf[inputInfo.width * inputInfo.pixelStride * h + inputInfo.pixelStride * w + ps];
    }
    inline void setTargetValue(T* buf, int w, int h, int ps, T v) {
        if(buf == nullptr || targetInfo.width == 0 || targetInfo.height == 0 || targetInfo.pixelStride == 0) {
            return;
        }
        buf[targetInfo.width * targetInfo.pixelStride * h + targetInfo.pixelStride * w + ps] = v;
    }

    ImageInfo inputInfo;
    ImageInfo targetInfo;
};

#endif //NOON_IMAGEFILTER_H
