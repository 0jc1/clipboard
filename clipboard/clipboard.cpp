// clipboard.cpp

#include <windows.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <list>
#include <fstream>
#include <shellapi.h> // For ShellExecuteA
#include <iomanip>    // For std::put_time
#include <ctime>      // For std::localtime
#include <sstream> // Add this for std::ostringstream

#include "cqueue.hpp"

// Custom message for tray icon events
#define WM_TRAYICON (WM_USER + 1)

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

// Window procedure to handle tray icon messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_TRAYICON) {
        if (lParam == WM_LBUTTONUP) { // Left mouse button released
            // Open clipboard_history.txt with the default editor
            ShellExecuteA(NULL, "open", "clipboard_history.txt", NULL, NULL, SW_SHOWNORMAL);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    std::string lastText;
	int capacity = 1000; // Max number of entries in history
	CQueue history(capacity);

    std::ofstream outFile("clipboard_history.txt", std::ios::app);

    if (outFile.is_open()) {
        std::cout << "Opened clipboard_history.txt.\n";
    } else {
        std::cerr << "Failed to open clipboard_history.txt for writing.\n";
        return -1;
	}

    // Register a window class for tray icon messages
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "ClipboardTrayClass";
    RegisterClassA(&wc);

    // Create a hidden window to receive tray icon events
    HWND hwnd = CreateWindowA("ClipboardTrayClass", "Clipboard Manager", 0, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);

	NOTIFYICONDATAA nid = {};
    //AllocConsole();
	// HWND h = FindWindowA("ConsoleWindowClass", NULL); // Not needed anymore
	nid.cbSize = sizeof(nid);
	nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;
    nid.uCallbackMessage = WM_TRAYICON;

	char toolTipText[] = "Clipboard Manager";
	memcpy(nid.szTip, toolTipText, sizeof(toolTipText)); // Tooltip text

    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Use default application icon

    Shell_NotifyIconA(NIM_ADD, &nid);

    std::cout << "Monitoring clipboard for new text...\n";

    // Start clipboard monitoring in a separate thread
    std::thread clipboardThread([&]() {
        while (true) {
            std::string currentText = GetClipboardText();
            if (!currentText.empty() && currentText != history.peek()) {
                lastText = currentText;
				history.push(currentText);

                // Save to file
                if (outFile.is_open()) {
                    outFile << GetCurrentTimeString() << " " << currentText << "\n";
                    outFile.flush(); 
                } else {
                    std::cerr << "Failed to open clipboard_history.txt for writing.\n";
                }

                std::cout << "New clipboard text:\n" << currentText << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });

    // Message loop to handle tray icon events
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    clipboardThread.join();
    outFile.close();
    return 0;
}
