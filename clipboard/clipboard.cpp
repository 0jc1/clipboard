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
	int capacity = 100; // Max number of entries in history
	BoundedQueue history(capacity);

    std::cout << "Monitoring clipboard for new text...\n";

    std::ofstream outFile("clipboard_history.txt", std::ios::app);

    while (true) {
        std::string currentText = GetClipboardText();
        if (!currentText.empty() && currentText != lastText) {
            std::cout << "New clipboard text:\n" << currentText << "\n";
            lastText = currentText;

            // Save to file
            if (outFile.is_open()) {
                outFile << currentText << "\n---\n"; // Separator between entries
                outFile.close();
            } else {
                std::cerr << "Failed to open clipboard_history.txt for writing.\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
