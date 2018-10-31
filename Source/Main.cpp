#include "resource.h"
#include <Richedit.h>
#include <ctime>
#include <string>
#include "Randomizer.h"
#include "windows.h"
#include "Version.h"

#define IDC_RANDOMIZE 0x401
#define IDC_TOGGLESPEED 0x402
#define IDC_SPEEDRUNNER 0x403
#define IDC_HARDMODE 0x404

HWND hwndSeed, hwndRandomize;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool wasSeedRandomlyGenerated;

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
	} else if (message == WM_COMMAND) {
		switch (LOWORD(wParam)) {
			// Speed checkbox
			case IDC_TOGGLESPEED:
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLESPEED)) {
					CheckDlgButton(hwnd, IDC_TOGGLESPEED, BST_UNCHECKED);
				} else {
					CheckDlgButton(hwnd, IDC_TOGGLESPEED, BST_CHECKED);
				}
				break;

			// Randomize button
			case IDC_RANDOMIZE:
			{
				std::wstring text(100, '\0');
				GetWindowText(hwndSeed, &text[0], 100);
				int seed = 0;
				if (wasSeedRandomlyGenerated || wcslen(text.c_str()) == 0) {
					srand(static_cast<unsigned int>(time(nullptr))); // Seed from the time in milliseconds
					rand(); // Increment RNG so that RNG isn't still using the time
					seed = rand() % 100000;
					std::wstring seedString = std::to_wstring(seed);
					SetWindowText(hwndSeed, seedString.c_str());
					wasSeedRandomlyGenerated = true;
				} else {
					seed = _wtoi(text.c_str());
					wasSeedRandomlyGenerated = false;
				}
				srand(seed);
				Randomizer randomizer;
				randomizer.Randomize();
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLESPEED)) {
					randomizer.AdjustSpeed();
				}
				SetWindowText(hwndRandomize, L"Randomized!");
				break;
			}
		}
	}
    return DefWindowProc(hwnd, message, wParam, lParam);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
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

	HWND hwnd = CreateWindow(WINDOW_CLASS, PRODUCT_NAME, WS_OVERLAPPEDWINDOW,
      400, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);

	CreateWindow(L"STATIC", L"Version: " VERSION_STR,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		390, 15, 90, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"STATIC", L"Enter a seed:",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		10, 15, 90, 16, hwnd, NULL, hInstance, NULL);
	hwndSeed = CreateWindow(MSFTEDIT_CLASS, L"",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 
        100, 10, 50, 26, hwnd, NULL, hInstance, NULL);
	hwndRandomize = CreateWindow(L"BUTTON", L"Randomize",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 10, 100, 26, hwnd, (HMENU)IDC_RANDOMIZE, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 52, 12, 12, hwnd, (HMENU)IDC_TOGGLESPEED, hInstance, NULL);
	CreateWindow(L"STATIC", L"Speed up various autoscrollers",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 50, 205, 16, hwnd, NULL, hInstance, NULL);

	/*
	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 52, 12, 12, hwnd, (HMENU)IDC_SPEEDRUNNER, hInstance, NULL);
	CreateWindow(L"STATIC", L"Allow hard-to-identify panels to be shuffled",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 50, 205, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 52, 12, 12, hwnd, (HMENU)IDC_HARDMODE, hInstance, NULL);
	CreateWindow(L"STATIC", L"Place harder puzzles in annoying spots",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 50, 205, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 52, 12, 12, hwnd, (HMENU)IDC_NORANDOMIZE, hInstance, NULL);
	CreateWindow(L"STATIC", L"Do not randomize any puzzles",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 50, 205, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 52, 12, 12, hwnd, (HMENU)IDC_CASUAL, hInstance, NULL);
	CreateWindow(L"STATIC", L"Don't randomize context-based puzzles",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 50, 205, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 52, 12, 12, hwnd, (HMENU)IDC_BANSNIPES, hInstance, NULL);
	CreateWindow(L"STATIC", L"Prevent sniping certain puzzles",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 50, 205, 16, hwnd, NULL, hInstance, NULL);
		
*/

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

    MSG msg;
    while (!GetMessage(&msg, nullptr, 0, 0) == 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
