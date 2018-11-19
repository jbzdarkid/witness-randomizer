#include "windows.h"
#include <Richedit.h>

#include <string>
#include <sstream>

#include "Version.h"
#include "Random.h"
#include "Randomizer.h"
#include "Panel.h"

#define IDC_RANDOMIZE 0x401
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

HWND hwndSeed, hwndRandomize;
// int panel = 0x18AF;
int panel = 0x33D4;
std::shared_ptr<Panel> _panel;
std::shared_ptr<Randomizer> randomizer = std::make_shared<Randomizer>();

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool seedIsRNG = false;

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
	} else if (message == WM_COMMAND || message == WM_TIMER) {
		switch (HIWORD(wParam)) {
			// Seed contents changed
			case EN_CHANGE:
				seedIsRNG = false;
		}
		switch (LOWORD(wParam)) {
			// Checkboxes
			case IDC_TOGGLESPEED:
				CheckDlgButton(hwnd, IDC_TOGGLESPEED, !IsDlgButtonChecked(hwnd, IDC_TOGGLESPEED));
				break;
			case IDC_TOGGLELASERS:
				CheckDlgButton(hwnd, IDC_TOGGLELASERS, !IsDlgButtonChecked(hwnd, IDC_TOGGLELASERS));
				break;
			case IDC_TOGGLESNIPES:
				CheckDlgButton(hwnd, IDC_TOGGLESNIPES, !IsDlgButtonChecked(hwnd, IDC_TOGGLESNIPES));
				break;

			// Randomize button
			case IDC_RANDOMIZE:
			{
				std::wstring text;
				text.reserve(100);
				GetWindowText(hwndSeed, &text[0], 100);
				int seed = _wtoi(text.c_str());

				// TODO: text needs to be resized!
				if (seedIsRNG || wcslen(text.c_str()) == 0) {
					seed = Random::RandInt(0, 100000);
					seedIsRNG = true;
				}

				randomizer->ClearOffsets();
				/* TODO:
				if (!randomizer->GameIsRunning()) {
					randomizer->StartGame(); // Try: CreateProcess(L"/path/to/TW.exe", ...);
				}
				*/
				if (randomizer->GameIsRandomized()) break;
				Random::SetSeed(seed);

				// Show seed and force redraw
 				std::wstring seedString = std::to_wstring(seed);
 				SetWindowText(hwndRandomize, L"Randomizing...");
 				SetWindowText(hwndSeed, seedString.c_str());
				RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);

				// Randomize, then apply settings
				randomizer->Randomize();
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLESPEED)) randomizer->AdjustSpeed();
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLELASERS)) randomizer->RandomizeLasers();
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLESNIPES)) randomizer->PreventSnipes();

				// Cleanup, and set timer for "randomization done"
				SetWindowText(hwndRandomize, L"Randomized!");
				SetTimer(hwnd, IDT_RANDOMIZED, 10000, NULL);
				break;
			}

			case IDT_RANDOMIZED:
				SetWindowText(hwndRandomize, L"Randomize");
				randomizer->GameIsRandomized(); // "Check" if the game is randomized to update the last known safe frame.
				break;
			case IDC_READ:
				_panel = std::make_shared<Panel>(panel);
				break;
			case IDC_RANDOM:
				_panel->Random();
				break;
			case IDC_WRITE:
				_panel->Write(panel);
				break;
			case IDC_DUMP:
				_panel->Serialize();
				break;
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

	RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);
	HWND hwnd = CreateWindow(WINDOW_CLASS, PRODUCT_NAME, WS_OVERLAPPEDWINDOW,
      rect.right - 550, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);

	CreateWindow(L"STATIC", L"Version: " VERSION_STR,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		390, 15, 90, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"STATIC", L"Enter a seed:",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		10, 15, 90, 16, hwnd, NULL, hInstance, NULL);
	hwndSeed = CreateWindow(MSFTEDIT_CLASS, L"",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
        100, 10, 50, 26, hwnd, NULL, hInstance, NULL);
	SendMessage(hwndSeed, EM_SETEVENTMASK, NULL, ENM_CHANGE); // Notify on text change

	hwndRandomize = CreateWindow(L"BUTTON", L"Randomize",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 10, 110, 26, hwnd, (HMENU)IDC_RANDOMIZE, hInstance, NULL);

#if GLOBALS == 0x5B28C0
	CreateWindow(L"BUTTON", L"READ",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 100, 100, 26, hwnd, (HMENU)IDC_READ, hInstance, NULL);
	CreateWindow(L"BUTTON", L"RANDOM",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 130, 100, 26, hwnd, (HMENU)IDC_RANDOM, hInstance, NULL);
	CreateWindow(L"BUTTON", L"WRITE",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 160, 100, 26, hwnd, (HMENU)IDC_WRITE, hInstance, NULL);
	CreateWindow(L"BUTTON", L"DUMP",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 190, 100, 26, hwnd, (HMENU)IDC_DUMP, hInstance, NULL);
#endif

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 52, 12, 12, hwnd, (HMENU)IDC_TOGGLESPEED, hInstance, NULL);
	CreateWindow(L"STATIC", L"Speed up various autoscrollers",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 50, 210, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 72, 12, 12, hwnd, (HMENU)IDC_TOGGLELASERS, hInstance, NULL);
	CreateWindow(L"STATIC", L"Randomize laser activations",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 70, 210, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		10, 92, 12, 12, hwnd, (HMENU)IDC_TOGGLESNIPES, hInstance, NULL);
	CheckDlgButton(hwnd, IDC_TOGGLESNIPES, TRUE);
	CreateWindow(L"STATIC", L"Prevent sniping certain puzzles",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		27, 90, 210, 16, hwnd, NULL, hInstance, NULL);

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
