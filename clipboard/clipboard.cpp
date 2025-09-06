// clipboard.cpp

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <list>
#include <fstream>

#include "BoundedQueue.hpp"


std::string GetClipboardText() {
    if (!OpenClipboard(nullptr))
        return "";

	HANDLE hData = GetClipboardData(CF_TEXT); //pointer to the clipboard data in CF_TEXT format
    if (hData == nullptr) {
        CloseClipboard();
        return "";
    }

    char* pszText = static_cast<char*>(GlobalLock(hData));

    GlobalUnlock(hData);
    CloseClipboard();

    if (pszText == nullptr)
        return "";

    return pszText;
}

int main() {
    std::string lastText;
	int capacity = 1000; // Max number of entries in history
	BoundedQueue history(capacity);

    std::ofstream outFile("clipboard_history.txt", std::ios::app);

    if (outFile.is_open()) {
        std::cout << "Opened clipboard_history.txt.\n";
    } else {
        std::cerr << "Failed to open clipboard_history.txt for writing.\n";
        return -1;
	}

    std::cout << "Monitoring clipboard for new text...\n";

    while (true) {
        std::string currentText = GetClipboardText();
        if (!currentText.empty() && currentText != lastText) {
            lastText = currentText;
			history.push(currentText);

            // Save to file
            if (outFile.is_open()) {
                outFile << currentText << "\n"; 
            } else {
                std::cerr << "Failed to open clipboard_history.txt for writing.\n";
            }

            std::cout << "New clipboard text:\n" << currentText << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    outFile.close();
    return 0;
}
