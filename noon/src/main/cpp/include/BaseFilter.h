//
// Created by newst on 2022-04-20.
//

#ifndef NOON_BASEFILTER_H
#define NOON_BASEFILTER_H

struct BaseInfo {
public:
    std::vector<std::vector<int>> shape;
    size_t getSize(unsigned int idx) {
        if (size == 0) {
            size_t result = 1;
            for(const int& dim : shape[idx]) {
                result *= dim;
            }
            size = result;
            return size;
        } else {
            return size;
        }
    }
private:
    size_t size = 0;
};

class BaseFilter {

};

#endif //NOON_BASEFILTER_H
