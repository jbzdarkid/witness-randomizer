#include "Windows.h"
#include "Richedit.h"
#include "Version.h"

#include <cassert>
#include <iostream>
#include <string>
#include <thread>

#include "Memory.h"
class Randomizer {
public:
    Randomizer(const std::shared_ptr<Memory>&) {}
    void Randomize() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
};

#define EXE_NAME L"witness64_d3d11.exe"
#define HEARTBEAT 0x401
#define RANDOMIZE_READY 0x402
#define RANDOMIZING 0403
#define RANDOMIZE_DONE 0x404

// Globals
HWND g_hwnd;
HWND g_seed;
HWND g_randomizerStatus;
HINSTANCE g_hInstance;
auto g_witnessProc = std::make_shared<Memory>();
std::shared_ptr<Randomizer> g_randomizer;

// Notifications I need:
// Game shutdown
// Load game?

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
    } else if (message == WM_COMMAND || message == WM_TIMER) {
        switch (LOWORD(wParam)) {
            case HEARTBEAT:
                SetTimer(g_hwnd, HEARTBEAT, 1000, NULL);
                // Potential improvement: Change this call to be part of the HEARTBEAT message.
                switch (g_witnessProc->Heartbeat(EXE_NAME)) {
                    case ProcStatus::NotRunning:
                        // Shut down randomizer, wait for startup
                        if (g_randomizer) g_randomizer = nullptr;
                        break;
                    case ProcStatus::Running:
                        if (!g_randomizer) {
                            g_randomizer = std::make_shared<Randomizer>(g_witnessProc);
                            PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_READY, NULL);
                        }
                        break;
                    case ProcStatus::NewGame: // This status will fire only once per new game
                        SetWindowText(g_seed, L"");
                        PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_READY, NULL);
                        break;
                    }
                break;
            case RANDOMIZE_READY:
                SetWindowText(g_randomizerStatus, L"Randomize");
                EnableWindow(g_randomizerStatus, TRUE);
                break;
            case RANDOMIZING:
                if (!g_randomizer) break;
                EnableWindow(g_randomizerStatus, FALSE);
                SetWindowText(g_randomizerStatus, L"Randomizing...");
                std::thread([]{
                    g_randomizer->Randomize();
                    PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_DONE, NULL);
                }).detach();
                break;
            case RANDOMIZE_DONE:
                SetWindowText(g_randomizerStatus, L"Randomized!");
                break;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND CreateLabel(int x, int y, int width, LPCWSTR text) {
	return CreateWindow(L"STATIC", text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		x, y, width, 16, g_hwnd, NULL, g_hInstance, NULL);
}

HWND CreateButton(int x, int y, int width, LPCWSTR text, int message) {
#pragma warning(push)
#pragma warning(disable: 4312)
	return CreateWindow(L"BUTTON", text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, width, 26, g_hwnd, (HMENU)message, g_hInstance, NULL);
#pragma warning(pop)
}

HWND CreateText(int x, int y, int width, LPCWSTR defaultText = L"") {
	return CreateWindow(MSFTEDIT_CLASS, L"",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
        x, y, width, 26, g_hwnd, NULL, g_hInstance, NULL);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	LoadLibrary(L"Msftedit.dll");
	WNDCLASSW wndClass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		WINDOW_CLASS,
		WINDOW_CLASS,
	};
	RegisterClassW(&wndClass);

    g_hInstance = hInstance;

	RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);
	g_hwnd = CreateWindow(WINDOW_CLASS, PRODUCT_NAME, WS_OVERLAPPEDWINDOW,
      rect.right - 550, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);

    CreateLabel(390, 15, 90, L"Version: " VERSION_STR);
    g_seed = CreateText(10, 10, 100);
    g_randomizerStatus = CreateButton(120, 10, 110, L"Randomize", RANDOMIZING);
    EnableWindow(g_randomizerStatus, FALSE);
    PostMessage(g_hwnd, WM_COMMAND, HEARTBEAT, NULL);

	ShowWindow(g_hwnd, nCmdShow);
	UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
