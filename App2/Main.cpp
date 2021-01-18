#include "pch.h"
#include "Richedit.h"
#include "Version.h"

#include "Memory.h"
#include "Random.h"
#include "Randomizer.h"
#include "Panels_.h"

#define HEARTBEAT 0x401
#define RANDOMIZE_READY 0x402
#define RANDOMIZING 0403
#define RANDOMIZE_CHALLENGE_DONE 0x405

// Globals
HWND g_hwnd;
HWND g_seed;
HWND g_randomizerStatus;
HINSTANCE g_hInstance;
auto g_witnessProc = std::make_shared<Memory>(L"witness64_d3d11.exe");
std::shared_ptr<Randomizer> g_randomizer;
void SetRandomSeed();

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
    } else if (message == WM_NOTIFY) {
        MSGFILTER* m = (MSGFILTER *)lParam;
        if (m->msg == WM_KEYDOWN && m->wParam == VK_RETURN) {
            if (IsWindowEnabled(g_randomizerStatus) == TRUE) {
                PostMessage(g_hwnd, WM_COMMAND, RANDOMIZING, NULL);
                return 1; // Non-zero to indicate that message was handled
            }
        }
    } else if (message == WM_COMMAND || message == WM_TIMER || message == WM_NOTIFY) {
        switch (LOWORD(wParam)) {
            case HEARTBEAT:
                switch ((ProcStatus)lParam) {
                    case ProcStatus::NotRunning:
                        // Shut down randomizer, wait for startup
                        if (g_randomizer) {
                            g_randomizer = nullptr;
                            EnableWindow(g_randomizerStatus, FALSE);
                        }
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
                SetWindowText(g_randomizerStatus, L"Randomize Challenge");
                break;
            case RANDOMIZING:
                if (!g_randomizer) break; // E.g. an enter press at the wrong time
                EnableWindow(g_randomizerStatus, FALSE);

                SetRandomSeed();
                std::thread([]{
                    SetWindowText(g_randomizerStatus, L"Randomizing Challenge...");
                    MEMORY_CATCH(g_randomizer->RandomizeChallenge());
                    PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_CHALLENGE_DONE, NULL);
                }).detach();
                break;
            case RANDOMIZE_CHALLENGE_DONE:
                EnableWindow(g_randomizerStatus, FALSE);
                SetWindowText(g_randomizerStatus, L"Randomized Challenge!");
                std::thread([]{
                    // Allow re-randomization for challenge -- it doesn't break the rest of the game.
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                    PostMessage(g_hwnd, WM_COMMAND, RANDOMIZE_READY, NULL);
                }).detach();
                break;
        }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void SetRandomSeed() {
    std::wstring text(128, L'\0');
    int length = GetWindowText(g_seed, text.data(), static_cast<int>(text.size()));
    if (length > 0) { // Set seed
        text.resize(length);
        Random::SetSeed(_wtoi(text.c_str()));
    } else { // Random seed
        int seed = Random::RandInt(0, 999999);

        text = std::to_wstring(seed);
        SetWindowText(g_seed, text.c_str());
        CHARRANGE range = {0, static_cast<long>(text.length())};
        PostMessage(g_seed, EM_EXSETSEL, NULL, (LPARAM)&range);
        SetFocus(g_seed);

		Random::SetSeed(seed);
    }
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
    g_seed = CreateText(10, 10, 100, L"21581");
    PostMessage(g_seed, EM_SETEVENTMASK, 0, ENM_KEYEVENTS);
    g_randomizerStatus = CreateButton(120, 10, 180, L"Randomize", RANDOMIZING);
    EnableWindow(g_randomizerStatus, FALSE);

    g_witnessProc->StartHeartbeat(g_hwnd, HEARTBEAT);

	ShowWindow(g_hwnd, nCmdShow);
	UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}
