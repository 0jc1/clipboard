#include "cqueue.hpp"
#include <iostream>

// Circular queue.

CQueue::CQueue(int cap) {
    capacity = cap; // assign capacity
    data = std::vector<std::string>(cap); // create vector with 'cap' elements
    head = -1;
    count = 0;
}

void CQueue::push(const std::string& value) {
	head = (++head) % capacity;
    data[head] = value;

    if (count < capacity) {
        count++;
    }
}

std::string CQueue::peek() {
    if (count == 0) {
        return "";
	}
    return data[head];
}


void CQueue::print() const {
    for (int i = 0; i < count; i++) {
        std::cout << data[i] << std::endl;
    }
    std::cout << std::endl;
}