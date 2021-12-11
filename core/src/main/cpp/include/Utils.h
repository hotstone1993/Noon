//
// Created by newst on 2021-12-11.
//

#ifndef NOON_UTILS_H
#define NOON_UTILS_H

enum {
    SUCCESS = 0,
    FAIL = -1
};

struct ImageInfo {
    int width;
    int height;
    int pixelStride;
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

#define TFLITE_MINIMAL_CHECK(x)                              \
if (!(x)) {                                                \
    fprintf(stderr, "Error at %s:%d\n", __FILE__, __LINE__); \
    return FAIL; \
}

#endif //NOON_UTILS_H
