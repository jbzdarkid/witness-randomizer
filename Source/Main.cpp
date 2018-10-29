// Source.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "tchar.h"
#include "Randomizer.h"
#include <Richedit.h>
#include <iostream>
#include <ctime>
#include <string>

#define MAX_LOADSTRING 100
#define IDC_RANDOMIZE 0x464

HINSTANCE hInst;
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hwndSeed, hwndRandomize;

// Forward declares
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	LoadStringW(hInstance, IDC_SOURCE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) {
        return FALSE;
    }

    MSG msg;
    while (!GetMessage(&msg, nullptr, 0, 0) == 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
	    0,
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOURCE)),
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		MAKEINTRESOURCEW(IDC_SOURCE),
		szWindowClass,
		LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)),
	};

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	WCHAR szTitle[MAX_LOADSTRING];
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      400, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) return FALSE;

	LoadLibrary(L"Msftedit.dll");
	HWND label = CreateWindow(L"STATIC", L"Enter a seed:",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		10, 15, 90, 16, hWnd, NULL, hInst, NULL);

	hwndSeed = CreateWindowEx(0, MSFTEDIT_CLASS, L"",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 
        100, 10, 50, 26, hWnd, NULL, hInst, NULL);

	hwndRandomize = CreateWindow(L"BUTTON", L"Randomize",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 10, 100, 26, hWnd, (HMENU)IDC_RANDOMIZE, hInst, NULL);

	HDC hdc = GetDC(hWnd);
	RECT rect;
	GetClientRect(hWnd, &rect);
	LPCWSTR text = L"this Window cannot be used";
	DrawText(hdc, text, static_cast<int>(wcslen(text)), &rect, DT_CENTER | DT_VCENTER);
	ReleaseDC(hWnd, hdc);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DESTROY) {
		PostQuitMessage(0);
	} else if (message == WM_COMMAND) {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
		if (wmId == IDC_RANDOMIZE) {
			std::wstring text(100, '\0');
			GetWindowText(hwndSeed, &text[0], 100);
			int seed = 0;
			if (wcslen(text.c_str()) == 0) {
				srand(static_cast<unsigned int>(time(nullptr))); // Seed from the time in milliseconds
				rand();
				seed = rand() % 100000;
				std::wstring seedString = std::to_wstring(seed);
				SetWindowText(hwndSeed, seedString.c_str());
			} else {
				seed = _wtoi(text.c_str());
			}
			srand(seed);
			Randomizer().Randomize();
			SetWindowText(hwndRandomize, L"Randomized!");
        }
	}
    return DefWindowProc(hWnd, message, wParam, lParam);
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}
