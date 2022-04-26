//
// Created by newst on 2022-04-20.
//

#ifndef NOON_BASEFILTER_H
#define NOON_BASEFILTER_H

#include <vector>

struct Node {
    std::string name = "";
    std::vector<int> shape;

    size_t& getSize() {
        if (size == 0) {
            size_t result = 1;
            for(const int& dim : shape) {
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

struct BaseInfo {
public:
    std::vector<Node> nodes;
    size_t getTotalSize() {
        if (size == 0) {
            size_t result = 0;
            for(Node& node : nodes) {
                result += node.getSize();
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
public:
    BaseFilter() {}
    virtual ~BaseFilter() {}
    virtual int setup(const BaseInfo& inputInfo, const BaseInfo& targetInfo) = 0;
    virtual int process(void* originalInput, void* originalOutput) = 0;
};

#endif //NOON_BASEFILTER_H
