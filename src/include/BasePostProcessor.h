//
// Created by newst on 2022-04-16.
//

#ifndef NOON_BASEPOSTPROCESSOR_H
#define NOON_BASEPOSTPROCESSOR_H


class BasePostProcessor {
public:
    BasePostProcessor() {}
    virtual ~BasePostProcessor() {}

    virtual int setup() = 0;
    virtual int process(void* input, void* output) = 0;
private:
};

#endif //NOON_BASEPOSTPROCESSOR_H