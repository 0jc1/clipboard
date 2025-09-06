#include "BoundedQueue.hpp"
#include <iostream>

BoundedQueue::BoundedQueue(int cap) {
    capacity = cap;              // assign capacity
    data = std::vector<std::string>(cap); // create vector with 'cap' elements
}

void BoundedQueue::push(std::string& value) {
    data[(head + count) % capacity] = value;
    if (count < capacity) {
        count++;
    }
    else {
        head = (head + 1) % capacity;  // overwrite oldest
    }
}

void BoundedQueue::print() const {
    for (int i = 0; i < count; i++) {
        std::cout << data[(head + i) % capacity] << " ";
    }
    std::cout << std::endl;
}