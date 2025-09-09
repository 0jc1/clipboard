#include "cqueue.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream> // Add this for std::ostringstream
#include <fstream> // Include fstream for std::ofstream

// Circular queue.

CQueue::CQueue(int cap) {
    capacity = cap; // assign capacity
    data = std::vector<Data>(cap); // create vector with 'cap' elements
    head = -1;
    count = 0;
}

void CQueue::push(const std::string& value) {
    Data d;
    d.text = value;
	d.time = GetCurrentTimeString();

	head = (++head) % capacity;
    data[head] = d;

    if (count < capacity) {
        count++;
    }
}

Data CQueue::peek() {
    if (count == 0) {
        return Data{};
	}
    return data[head];
}

void CQueue::print() const {
    for (int i = 0; i < count; i++) {
        std::cout << data[i].text << std::endl;
    }
    std::cout << std::endl;
}

// Returns current time as a formatted string
std::string GetCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_s(&tm_buf, &now_c); // Use thread-safe version
    std::ostringstream oss;
    oss << "[" << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S") << "]";
    return oss.str();
}

void CQueue::printFile(std::ofstream& ofs) const {
    if (count == 0) {
        ofs << "(empty)\n";
        return;
    }

    for (int i = 0; i < count; i++) {
        ofs << "[" << data[i].time << "]: " << data[i].text << "\n";
    }
}