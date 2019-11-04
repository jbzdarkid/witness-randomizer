#include "Windows.h"
#include "Richedit.h"
#include "Version.h"

#include <cassert>
#include <iostream>

#include "Memory.h"

#define EXE_NAME L"witness64_d3d11.exe"

#define PROC_ATTACH 0x401
#define IDC_TOGGLESPEED 0x402
#define IDC_SPEEDRUNNER 0x403
#define IDC_HARDMODE 0x404
#define IDC_READ 0x405
#define IDC_RANDOM 0x406
#define IDC_WRITE 0x407
#define IDC_DUMP 0x408
#define IDT_RANDOMIZED 0x409
#define IDC_TOGGLELASERS 0x410
#define IDC_TOGGLESNIPES 0x411

// Globals
HWND g_hwnd;
HINSTANCE g_hInstance;
auto g_witnessProc = std::make_shared<Memory>();

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
    } else if (message == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case PROC_ATTACH:
                if (!g_witnessProc->Initialize(EXE_NAME)) {
                    OutputDebugString(L"Failed to initialize, trying again in 5 seconds");
                    SetTimer(g_hwnd, PROC_ATTACH, 5000, NULL); // Re-attach in 10s
                }
                break;
            case IDC_TOGGLELASERS:
                OutputDebugString(L"Hello, world!");
                break;
            default:
                assert(false);
                break;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void CreateLabel(int x, int y, int width, LPCWSTR text) {
	CreateWindow(L"STATIC", text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		x, y, width, 16, g_hwnd, NULL, g_hInstance, NULL);
}

void CreateButton(int x, int y, int width, LPCWSTR text, int message) {
#pragma warning(push)
#pragma warning(disable: 4312)
	CreateWindow(L"BUTTON", text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, width, 26, g_hwnd, (HMENU)message, g_hInstance, NULL);
#pragma warning(pop)
}

/*
	hwndSeed = CreateWindow(MSFTEDIT_CLASS, L"",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
        100, 10, 50, 26, hwnd, NULL, hInstance, NULL);
*/

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
    CreateButton(10, 72, 100, L"Button text", IDC_TOGGLELASERS);
    PostMessage(g_hwnd, WM_COMMAND, PROC_ATTACH, NULL);

	ShowWindow(g_hwnd, nCmdShow);
	UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
