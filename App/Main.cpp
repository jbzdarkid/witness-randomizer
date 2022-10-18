// Files from the App project
#include "Version.h"
#include "Windows.h"
// #include "Randomizer.h"

// Files from the C++ std library
#include <memory>

// Files from the Randomizer Library
#include "Memory.h"
#include "Randomizer.h"
#include "PanelOffsets.h"

#define HEARTBEAT 0x400
#define IDC_RANDOMIZE 0x401

// Globals
using namespace std;
HWND g_hwnd, g_randomize;
auto g_memory = make_shared<Memory>(L"witness64_d3d11.exe");

// As opposed to the 6.x randomizer, this app is more of a 'demo' than an actual attempt at a randomizer.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CTLCOLORSTATIC:
    {
        // Get rid of the gross gray background. https://stackoverflow.com/a/4495814
        SetTextColor((HDC)wParam, RGB(0, 0, 0));
        SetBkColor((HDC)wParam, RGB(255, 255, 255));
        SetBkMode((HDC)wParam, OPAQUE);
        static HBRUSH s_solidBrush = CreateSolidBrush(RGB(255, 255, 255));
        return (LRESULT)s_solidBrush;
    }
    case HEARTBEAT: // The "heartbeat" of the game -- tells you if The Witness is running, stopped, loading, etc.
        switch ((ProcStatus)wParam) {
            case ProcStatus::Stopped:
                EnableWindow(g_randomize, false); // Prevent randomization when the game stops
                break;
            case ProcStatus::Started:
                EnableWindow(g_randomize, true); // Allow randomization when the game starts
                break;
            case ProcStatus::NotRunning:
            case ProcStatus::Reload:
            case ProcStatus::NewGame:
                break; // No specific actions needed. You might consider reloading the randomization when a player loads a save, e.g.
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(message)) {
        case IDC_RANDOMIZE:
            Randomizer randomizer(g_memory);

            if (randomizer.ReadPanelData<int>(0x00064, NUM_DOTS) > 5) {
				if (MessageBox(hwnd, L"Game is currently randomized. Are you sure you want to randomize again? (Can cause glitches)", NULL, MB_YESNO) != IDYES) break;
			}
            break;
        }
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

time_t lastShownAssert = ~0ULL; // MAXINT
void ShowAssertDialogue() {
    // Only show an assert every 30 seconds. This prevents assert loops inside the WndProc, as well as adding a grace period after an assert fires.
    if (time(nullptr) - lastShownAssert < 30) return;
    lastShownAssert = time(nullptr);
    wstring msg = WINDOW_CLASS L" version " VERSION_STR L" has encountered an error.\n";
    msg += L"Please press Control C to copy this error, and paste it to darkid.\n";
    MessageBox(NULL, msg.c_str(), WINDOW_CLASS L" encountered an error.", MB_TASKMODAL | MB_ICONHAND | MB_OK | MB_SETFOREGROUND);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    WNDCLASSW wndClass = {
      CS_HREDRAW | CS_VREDRAW,
      WndProc,
      0,
      0,
      hInstance,
      NULL,
      LoadCursor(nullptr, IDC_ARROW),
      (HBRUSH)(COLOR_WINDOW + 1),
      WINDOW_CLASS,
      WINDOW_CLASS,
    };
    RegisterClassW(&wndClass);

    RECT rect;
    GetClientRect(GetDesktopWindow(), &rect);
    g_hwnd = CreateWindow(WINDOW_CLASS, PRODUCT_NAME, WS_OVERLAPPEDWINDOW,
        rect.right - 550, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);

    g_randomize = CreateWindow(L"BUTTON", L"Randomize!",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        0, 0, 100, 26,
        g_hwnd, (HMENU)IDC_RANDOMIZE, hInstance, NULL);

    g_memory->StartHeartbeat(g_hwnd, HEARTBEAT);

    ShowWindow(g_hwnd, nCmdShow);
    UpdateWindow(g_hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) == TRUE) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
