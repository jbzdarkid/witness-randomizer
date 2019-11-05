#include "Windows.h"
#include "Richedit.h"
#include "Version.h"

#include <cassert>
#include <iostream>
#include <string>
#include <thread>

#include "Memory.h"
#include <Random.h>
class Randomizer {
public:
    Randomizer(const std::shared_ptr<Memory>&) {}
    void Randomize(int seed) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    void RandomizeChallenge(int seed) {
        Randomize(seed);
    }
};

// Heartbeat is defined to 0x401 by Memory.h
#define RANDOMIZE_READY 0x402
#define RANDOMIZING 0403
#define RANDOMIZE_DONE 0x404
#define RANDOMIZE_CHALLENGE_DONE 0x405
#define CHALLENGE_ONLY 0x406

// Globals
HWND g_hwnd;
HWND g_seed;
HWND g_randomizerStatus;
HINSTANCE g_hInstance;
auto g_witnessProc = std::make_shared<Memory>(L"witness64_d3d11.exe");
std::shared_ptr<Randomizer> g_randomizer;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
    } else if (message == WM_COMMAND || message == WM_TIMER) {
        switch (LOWORD(wParam)) {
            case HEARTBEAT:
                switch ((ProcStatus)lParam) {
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
                EnableWindow(g_randomizerStatus, TRUE);
                if (IsDlgButtonChecked(hwnd, CHALLENGE_ONLY)) {
                    SetWindowText(g_randomizerStatus, L"Randomize Challenge");
                } else {
                    SetWindowText(g_randomizerStatus, L"Randomize");
                }
                break;
            case RANDOMIZING:
                if (!g_randomizer) {
                    assert(false);
                    break;
                }
                EnableWindow(g_randomizerStatus, FALSE);

                {
                    int seed = 0;
                    std::wstring text(128, L'\0');
                    int size = GetWindowText(g_seed, text.data(), static_cast<int>(text.size()));
                    if (size > 0) { // Set seed
                        seed = _wtoi(text.c_str());
                    } else { // Random seed
                        seed = Random::RandInt(0, 999999);
                        SetWindowText(g_seed, std::to_wstring(seed).c_str());
				        RedrawWindow(g_seed, NULL, NULL, RDW_UPDATENOW);
                    }
                    std::thread([hwnd, seed]{
                        if (IsDlgButtonChecked(hwnd, CHALLENGE_ONLY)) {
                            SetWindowText(g_randomizerStatus, L"Randomizing Challenge...");
                            g_randomizer->RandomizeChallenge(seed);
                            PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_CHALLENGE_DONE, NULL);
                        } else {
                            SetWindowText(g_randomizerStatus, L"Randomizing...");
                            g_randomizer->Randomize(seed);
                            PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_DONE, NULL);
                        }
                    }).detach();
                }
                break;
            case RANDOMIZE_DONE:
                EnableWindow(g_randomizerStatus, FALSE);
                SetWindowText(g_randomizerStatus, L"Randomized!");
                break;
            case RANDOMIZE_CHALLENGE_DONE:
                EnableWindow(g_randomizerStatus, FALSE);
                SetWindowText(g_randomizerStatus, L"Randomized Challenge!");
                break;
            case CHALLENGE_ONLY:
                CheckDlgButton(hwnd, CHALLENGE_ONLY, !IsDlgButtonChecked(hwnd, CHALLENGE_ONLY));
                if (IsWindowEnabled(g_randomizerStatus)) {
                    PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_READY, NULL);
                }
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

HWND CreateText(int x, int y, int width, LPCWSTR defaultText = L"") {
	return CreateWindow(MSFTEDIT_CLASS, defaultText,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
        x, y, width, 26, g_hwnd, NULL, g_hInstance, NULL);
}

#pragma warning(push)
#pragma warning(disable: 4312)
HWND CreateButton(int x, int y, int width, LPCWSTR text, int message) {
	return CreateWindow(L"BUTTON", text,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		x, y, width, 26, g_hwnd, (HMENU)message, g_hInstance, NULL);
}

HWND CreateCheckbox(int x, int y, int message) {
	return CreateWindow(L"BUTTON", L"",
        WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
        x, y, 12, 12, g_hwnd, (HMENU)message, g_hInstance, NULL);
}
#pragma warning(pop)

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
    g_randomizerStatus = CreateButton(120, 10, 180, L"Randomize", RANDOMIZING);
    EnableWindow(g_randomizerStatus, FALSE);
    CreateCheckbox(10, 300, CHALLENGE_ONLY);
    CreateLabel(30, 300, 200, L"Randomize the challenge only");
    EnableWindow(g_randomizerStatus, FALSE);

    g_witnessProc->StartHeartbeat(g_hwnd);

	ShowWindow(g_hwnd, nCmdShow);
	UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
