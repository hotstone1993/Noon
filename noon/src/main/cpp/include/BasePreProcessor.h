//
// Created by newst on 2022-04-16.
//

#ifndef NOON_BASEPREPROCESSOR_H
#define NOON_BASEPREPROCESSOR_H

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
class BasePreProcessor {
public:
    BasePreProcessor() {}
    virtual ~BasePreProcessor() {}

    virtual int setup() = 0;
    virtual int inference(INTPUT_TYPE* inputBuffer, INTPUT_TYPE* output) = 0;
private:
};

#endif //NOON_BASEPREPROCESSOR_H
