#pragma once

#include <vector>
#include <string>
#include <fstream>


/**
 * Custom circular queue 
 *
 * @note This class is not thread-safe.
 * 
 * @see https://en.wikipedia.org/wiki/Circular_buffer
 *
 */

std::string GetCurrentTimeString();

struct Data {
    std::string text;
    std::string time;
};

class CQueue {
    std::vector<Data> data;
    int head = 0;
    int count = 0;
public:
    int capacity = 100;
    explicit CQueue(int cap);
    void push(const std::string& value);
    void print() const;           // Print to std::cout
    void printFile(std::ofstream& ofs) const; // Print to file
    Data peek();
};
