//
// Created by newst on 2022-04-16.
//

#ifndef NOON_BASEPREPROCESSOR_H
#define NOON_BASEPREPROCESSOR_H

class BasePreProcessor {
public:
    BasePreProcessor() {}
    virtual ~BasePreProcessor() {}

    virtual int setup() = 0;
    virtual int process(void* input, void* output) = 0;
private:
};

#endif //NOON_BASEPREPROCESSOR_H
