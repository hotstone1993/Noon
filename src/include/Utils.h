//
// Created by newst on 2021-12-11.
//

#ifndef NOON_UTILS_H
#define NOON_UTILS_H


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

#define DELETE_VOID_POINTER(X, TYPE) { \
    TYPE* temp = static_cast<TYPE*>(X); \
    if(temp != nullptr) { \
        delete[] temp;     \
        X = nullptr;    \
    }                   \
}

#define ALLOC_CLASS(X, CLASS, TYPE) { \
    if(X == nullptr) {                \
        X = new CLASS<TYPE>();        \
    }                  \
}

#define ALLOC_BUFFER(BUFFER, TYPE, SIZE) { \
    if (BUFFER == nullptr) {   \
        BUFFER = new TYPE[SIZE];\
    }    \
}

#endif //NOON_UTILS_H
