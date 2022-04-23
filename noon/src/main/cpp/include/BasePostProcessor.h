//
// Created by newst on 2022-04-16.
//

#ifndef NOON_BASEPOSTPROCESSOR_H
#define NOON_BASEPOSTPROCESSOR_H

template <typename INTPUT_TYPE, typename OUTPUT_TYPE>
class BasePostProcessor {
public:
    BasePostProcessor() {}
    virtual ~BasePostProcessor() {}

    virtual int setup() = 0;
    virtual int inference(OUTPUT_TYPE** inputBuffer, OUTPUT_TYPE** output) = 0;
private:
};

#endif //NOON_BASEPOSTPROCESSOR_H