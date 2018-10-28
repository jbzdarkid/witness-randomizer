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
HWND hwndSeed;
std::shared_ptr<Randomizer> randomizer;

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
	randomizer = std::make_shared<Randomizer>();
	if (randomizer == nullptr) {
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
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	LoadLibrary(L"Msftedit.dll");
	HWND label = CreateWindow(L"BUTTON", L"Enter a seed:",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		10, 10, 100, 26, hWnd, NULL, hInst, NULL);

	hwndSeed = CreateWindowEx(0, MSFTEDIT_CLASS, L"",
        ES_MULTILINE | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP, 
        120, 10, 50, 26, hWnd, NULL, hInst, NULL);

	HWND hwndRandomize = CreateWindow(L"BUTTON", L"Randomize",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		180, 10, 100, 26, hWnd, (HMENU)IDC_RANDOMIZE, hInst, NULL);

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
	if (WM_COMMAND == WM_DESTROY) { 
        PostQuitMessage(0);
	} else {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
		case IDC_RANDOMIZE:
		{
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
			randomizer->Randomize(seed);

			break;
		}
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
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
