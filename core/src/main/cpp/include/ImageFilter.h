//
// Created by newst on 2021-12-11.
//

#ifndef NOON_IMAGEFILTER_H
#define NOON_IMAGEFILTER_H

#include "Utils.h"

template <typename T>
class ImageFilter {
public:
    ImageFilter() {}
    virtual ~ImageFilter() {}

    virtual int setup(ImageInfo inputInfo, ImageInfo targetInfo) = 0;
    virtual int process(T* input, T* output) = 0;
protected:
    ImageInfo inputInfo;
    ImageInfo targetInfo;
};

#endif //NOON_IMAGEFILTER_H
