#pragma once

#include <vector>
#include <string>

class BoundedQueue {
    std::vector<std::string> data;
    int head = 0;
    int count = 0;
    int capacity;
public:
    explicit BoundedQueue(int cap);
    void push(std::string& value);
    void print() const;
};
