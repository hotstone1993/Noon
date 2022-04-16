//
// Created by newst on 2021-12-11.
//

#ifndef NOON_UTILS_H
#define NOON_UTILS_H

struct ImageInfo {
    int width;
    int height;
    int pixelStride;
};

struct Rectrangle {
    int width;
    int height;
};

#define DELETE(X) { \
    if(X != nullptr) { \
        delete X;       \
        X = nullptr;    \
    }                   \
}

#define DELETE_ARRAY(X) { \
    if(X != nullptr) { \
        delete[] X;     \
        X = nullptr;    \
    }                   \
}

#define ALLOC_BUFFER(BUFFER, TYPE, SIZE) { \
    if (BUFFER == nullptr) {   \
        BUFFER = new TYPE[SIZE];\
    }    \
}

#endif //NOON_UTILS_H
