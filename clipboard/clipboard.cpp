// clipboard.cpp

#include <windows.h>
#include <winreg.h> // For registry functions
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


#define WM_TRAYICON (WM_USER + 1) // Custom message for tray icon events
#define CAPACITY 100 // Capacity of clipboard history

std::string lastText;
CQueue *history = new CQueue(CAPACITY);
std::ofstream outFile;


//void writeToFile(const std::string& text) {
//    // Close and reopen the file in truncate mode to clear it
//    if (outFile.is_open()) {
//        outFile.close();
//    }
//    outFile.open("clipboard_history.txt", std::ios::trunc);
//    
//    if (outFile.is_open()) {
//        // Write the new text with timestamp
//        history->printFile(outFile); 
//        outFile.flush();
//    } else {
//        std::cerr << "Failed to open clipboard_history.txt for writing.\n";
//    }
//}

void writeToFile(const std::string& text) {
    if (outFile.is_open()) {
        outFile << "[" << GetCurrentTimeString() << "]: " << text << std::endl;
        outFile.flush();
    } else {
        std::cerr << "Failed to open clipboard_history.txt for writing.\n";
    }
}

// Add this function near the top of the file, after the includes
bool SetStartupRun(bool enable) {
    HKEY hKey;
    const char* keyPath = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "ClipboardManager";
    
    // Get the full path of the executable
    char exePath[MAX_PATH];
    if (GetModuleFileNameA(NULL, exePath, MAX_PATH) == 0) {
        return false;
    }

    // Open the registry key
    if (RegOpenKeyExA(HKEY_CURRENT_USER, keyPath, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        return false;
    }

    bool success = false;
    if (enable) {
        // Add to startup
        success = (RegSetValueExA(hKey, valueName, 0, REG_SZ, 
            (const BYTE*)exePath, strlen(exePath) + 1) == ERROR_SUCCESS);
    } else {
        // Remove from startup
        success = (RegDeleteValueA(hKey, valueName) == ERROR_SUCCESS);
    }

    RegCloseKey(hKey);
    return success;
}
// Window procedure to handle tray icon messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch(uMsg) {
        case WM_CREATE:
            AddClipboardFormatListener(hwnd);
            break;
        case WM_TRAYICON:
            if (lParam == WM_LBUTTONUP) { // Left mouse button released
                // Open clipboard_history.txt with the default editor
                ShellExecuteA(NULL, "open", "clipboard_history.txt", NULL, NULL, SW_SHOWNORMAL);
            }
            break;
        case WM_CLIPBOARDUPDATE:
            if (OpenClipboard(nullptr)) {
                HANDLE hData = GetClipboardData(CF_TEXT);
                if (hData) {
                    char* pszText = (char*)GlobalLock(hData);
                    GlobalUnlock(hData);
                    if (pszText != nullptr) {
                        std::string text = pszText ? pszText : "";
                        if (lastText != text && !text.empty()) {
                            std::cout << "New text: " << text << std::endl;
							history->push(text);
							writeToFile(text);
                            lastText = text;
                        }
					}
                }
                CloseClipboard();
            }
            break;
        case WM_DESTROY:
            RemoveClipboardFormatListener(hwnd);
            PostQuitMessage(0);
            break;
        default:
            break;
	}

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    outFile = std::ofstream("clipboard_history.txt", std::ios::app);

    if (outFile.is_open()) {
        std::cout << "Opened clipboard_history.txt.\n";
    } else {
        std::cerr << "Failed to open clipboard_history.txt for writing.\n";
        return -1;
	}

    if (!SetStartupRun(true)) {
        std::cerr << "Failed to set startup registry entry.\n";
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

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    outFile.close();
    return 0;
}
