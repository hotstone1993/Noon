//
// Created by newst on 2022-04-26.
//

#ifndef NOON_BASEML_H
#define NOON_BASEML_H
#include "Utils.h"
#include <cstdint>

enum {
    CPU = 0,
    GPU = 1,
    UNKNOWN_DELEGATE = 2
};

typedef enum {
    NoonFloat64 = 0,
    NoonFloat32 = 1,
    NoonInt64 = 2,
    NoonUInt64 = 3,
    NoonInt32 = 4,
    NoonUInt32 = 5,
    NoonInt16 = 6,
    NoonUInt16 = 7,
    NoonInt8 = 8,
    NoonUInt8 = 9,
    NoonUnknown = 10
} NoonType;

inline int getNoonTypeSize(NoonType type) {
    if (type == NoonFloat64 || type == NoonInt64 || type == NoonUInt64) {
        return 64;
    } else if (type == NoonFloat32 || type == NoonInt32 || type == NoonUInt32) {
        return 32;
    } else if (type == NoonInt16 || type == NoonUInt16) {
        return 16;
    } else if (type == NoonInt8 || type == NoonUInt8) {
        return 8;
    } else {
        return 0;
    }
}

inline void allocBuffer(void** buffer, NoonType type, unsigned int size) {
    if (type == NoonFloat64) {
        ALLOC_BUFFER(*buffer, double, size)
    } else if (type == NoonFloat32) {
        ALLOC_BUFFER(*buffer, float, size)
    } else if (type == NoonInt64) {
        ALLOC_BUFFER(*buffer, int64_t, size)
    } else if (type == NoonUInt64) {
        ALLOC_BUFFER(*buffer, uint64_t, size)
    } else if (type == NoonInt32) {
        ALLOC_BUFFER(*buffer, int32_t, size)
    } else if (type == NoonUInt32) {
        ALLOC_BUFFER(*buffer, uint32_t, size)
    } else if (type == NoonInt16) {
        ALLOC_BUFFER(*buffer, int16_t, size)
    } else if (type == NoonUInt16) {
        ALLOC_BUFFER(*buffer, uint16_t, size)
    } else if (type == NoonInt8) {
        ALLOC_BUFFER(*buffer, int8_t, size)
    } else if (type == NoonUInt8) {
        ALLOC_BUFFER(*buffer, uint8_t, size)
    }
}

#define ALLOC_TEMPLATE_CLASS(target, className, type) {\
    if (type == NoonFloat64) {\
        ALLOC_CLASS(target, className, double); \
    } else if (type == NoonFloat32) {\
        ALLOC_CLASS(target, className, float); \
    } else if (type == NoonInt64) { \
        ALLOC_CLASS(target, className, int64_t); \
    } else if (type == NoonUInt64) {\
        ALLOC_CLASS(target, className, uint64_t); \
    } else if (type == NoonInt32) {\
        ALLOC_CLASS(target, className, int32_t); \
    } else if (type == NoonUInt32) {\
        ALLOC_CLASS(target, className, uint32_t); \
    } else if (type == NoonInt16) {\
        ALLOC_CLASS(target, className, int16_t); \
    } else if (type == NoonUInt16) {\
        ALLOC_CLASS(target, className, uint16_t); \
    } else if (type == NoonInt8) {\
        ALLOC_CLASS(target, className, int8_t); \
    } else if (type == NoonUInt8) {\
        ALLOC_CLASS(target, className, uint8_t); \
    }\
};

struct BaseMLInfo {
    int delegateType;
};

class BaseML {
public:
    virtual int loadModel(const char* file, size_t fileSize, BaseMLInfo& info) = 0;
    virtual void getType(NoonType& input, NoonType& output) = 0;
};

#endif //NOON_BASEML_H
