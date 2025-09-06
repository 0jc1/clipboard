#pragma once

#include <vector>
#include <string>


/**
 * Circular queue.
 *
 * @note This class is not thread-safe.
 * 
 * @see https://en.wikipedia.org/wiki/Circular_buffer
 *
 */

class CQueue {
    std::vector<std::string> data;
    int head = 0;
    int count = 0;
    int capacity = 100;
public:
    explicit CQueue(int cap);
    void push(const std::string& value);
    void print() const;
    std::string peek();
};
