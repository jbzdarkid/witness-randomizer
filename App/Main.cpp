#include "windows.h"
#include <Richedit.h>

#include <string>
#include <sstream>
#include <iostream>

#include "Version.h"
#include "Random.h"
#include "Randomizer.h"
#include "Panel.h"
#include "Generate.h"
#include "PuzzleList.h"

#define IDC_RANDOMIZE 0x401
#define IDC_TOGGLESPEED 0x402
#define IDC_SPEEDRUNNER 0x403
#define IDC_HARDMODE 0x404
#define IDC_READ 0x405
#define IDC_TEST 0x406
#define IDC_WRITE 0x407
#define IDC_DUMP 0x408
#define IDT_RANDOMIZED 0x409
#define IDC_TOGGLELASERS 0x410
#define IDC_TOGGLESNIPES 0x411

#define IDC_ADD 0x301
#define IDC_REMOVE 0x302
#define IDC_ROTATED 0x303
#define IDC_NEGATIVE 0x304
#define IDC_SYMMETRYX 0x305
#define IDC_SYMMETRYY 0x306

#define SHAPE_11 0x1000
#define SHAPE_12 0x2000
#define SHAPE_13 0x4000
#define SHAPE_14 0x8000
#define SHAPE_21 0x0100
#define SHAPE_22 0x0200
#define SHAPE_23 0x0400
#define SHAPE_24 0x0800
#define SHAPE_31 0x0010
#define SHAPE_32 0x0020
#define SHAPE_33 0x0040
#define SHAPE_34 0x0080
#define SHAPE_41 0x0001
#define SHAPE_42 0x0002
#define SHAPE_43 0x0004
#define SHAPE_44 0x0008

HWND hwndSeed, hwndRandomize, hwndCol, hwndRow, hwndElem, hwndColor, hwndLoadingText;

//int panel = 0x00020; // Outside Tutorial Stones Tutorial 8
//int panel = 0x0A3B2; // Tutorial Back Right (2 start points)
int panel = 0x00060; // Outside Tutorial Dots Tutorial 4

std::shared_ptr<Panel> _panel = std::make_shared<Panel>();
std::shared_ptr<Randomizer> randomizer = std::make_shared<Randomizer>();
std::shared_ptr<Generate> generator = std::make_shared<Generate>();

int ctr = 0;
TCHAR text[30];
int x, y;
std::wstring str;
Decoration::Shape symbol;
Decoration::Color color;
int currentShape;
std::vector<long long> shapePos = { SHAPE_11, SHAPE_12, SHAPE_13, SHAPE_14, SHAPE_21, SHAPE_22, SHAPE_23, SHAPE_24,
							  SHAPE_31, SHAPE_32, SHAPE_33, SHAPE_34, SHAPE_41, SHAPE_42, SHAPE_43, SHAPE_44 };
std::vector<long long> defaultShape = { SHAPE_21, SHAPE_31, SHAPE_32, SHAPE_33 };

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
				//if (randomizer->GameIsRandomized()) break;
				//Random::SetSeed(seed);
				srand(seed);

				// Show seed and force redraw
 				std::wstring seedString = std::to_wstring(seed);
 				SetWindowText(hwndRandomize, L"Randomizing...");
 				SetWindowText(hwndSeed, seedString.c_str());
				RedrawWindow(hwnd, NULL, NULL, RDW_UPDATENOW);

				// Randomize, then apply settings
				//randomizer->Randomize();
				randomizer->GenerateNormal(hwndLoadingText);
				/*
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLESPEED)) randomizer->AdjustSpeed();
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLELASERS)) randomizer->RandomizeLasers();
				if (IsDlgButtonChecked(hwnd, IDC_TOGGLESNIPES)) randomizer->PreventSnipes();
				*/

				randomizer->AdjustSpeed();
				randomizer->PreventSnipes();

				// Cleanup, and set timer for "randomization done"
				SetWindowText(hwndRandomize, L"Randomized!");
				SetTimer(hwnd, IDT_RANDOMIZED, 10000, NULL);
				break;
			}

			case IDT_RANDOMIZED:
				SetWindowText(hwndRandomize, L"Randomize");
				randomizer->GameIsRandomized(); // "Check" if the game is randomized to update the last known safe frame.
				break;
			
			case IDC_ADD:
				memset(&text, 0, sizeof(text));
				GetWindowText(hwndElem, text, 30);
				if (wcscmp(text, L"Stone") == 0) symbol = Decoration::Shape::Stone;
				if (wcscmp(text, L"Star") == 0) symbol = Decoration::Shape::Star;
				if (wcscmp(text, L"Eraser") == 0) symbol = Decoration::Shape::Eraser;
				if (wcscmp(text, L"Shape") == 0) symbol = Decoration::Shape::Poly;
				if (wcscmp(text, L"Triangle 1") == 0) symbol = Decoration::Shape::Triangle1;
				if (wcscmp(text, L"Triangle 2") == 0) symbol = Decoration::Shape::Triangle2;
				if (wcscmp(text, L"Triangle 3") == 0) symbol = Decoration::Shape::Triangle3;
				if (wcscmp(text, L"Dot (Intersection)") == 0) symbol = Decoration::Shape::Dot_Intersection;
				if (wcscmp(text, L"Dot (Row)") == 0) symbol = Decoration::Shape::Dot_Row;
				if (wcscmp(text, L"Dot (Column)") == 0) symbol = Decoration::Shape::Dot_Column;
				if (wcscmp(text, L"Gap (Row)") == 0) symbol = Decoration::Shape::Gap_Row;
				if (wcscmp(text, L"Gap (Column)") == 0) symbol = Decoration::Shape::Gap_Column;
				if (wcscmp(text, L"Start") == 0) symbol = Decoration::Shape::Start;
				if (wcscmp(text, L"Exit") == 0) symbol = Decoration::Shape::Exit;
				memset(&text, 0, sizeof(text));
				GetWindowText(hwndColor, text, 30);
				if (wcscmp(text, L"Black") == 0) color = Decoration::Color::Black;
				if (wcscmp(text, L"White") == 0) color = Decoration::Color::White;
				if (wcscmp(text, L"Red") == 0) color = Decoration::Color::Red;
				if (wcscmp(text, L"Orange") == 0) color = Decoration::Color::Orange;
				if (wcscmp(text, L"Yellow") == 0) color = Decoration::Color::Yellow;
				if (wcscmp(text, L"Green") == 0) color = Decoration::Color::Green;
				if (wcscmp(text, L"Cyan") == 0) color = Decoration::Color::Cyan;
				if (wcscmp(text, L"Blue") == 0) color = Decoration::Color::Blue;
				if (wcscmp(text, L"Purple") == 0) color = Decoration::Color::Purple;
				if (wcscmp(text, L"Magenta") == 0) color = Decoration::Color::Magenta;
				if (wcscmp(text, L"None") == 0) color = Decoration::Color::None;
				if (wcscmp(text, L"???") == 0) color = Decoration::Color::X;
				str.reserve(30);
				GetWindowText(hwndCol, &str[0], 30);
				x = _wtoi(str.c_str());
				GetWindowText(hwndRow, &str[0], 30);
				y = _wtoi(str.c_str());
				
				_panel->Read(panel);
				if (symbol == Decoration::Shape::Poly) {
					_panel->SetShape(x, y, currentShape, IsDlgButtonChecked(hwnd, IDC_ROTATED), IsDlgButtonChecked(hwnd, IDC_NEGATIVE), color);
				}
				else _panel->SetSymbol(x, y, symbol, color);
				_panel->Write(panel);
				break;

			case IDC_REMOVE:
				GetWindowText(hwndCol, &str[0], 30);
				x = _wtoi(str.c_str());
				GetWindowText(hwndRow, &str[0], 30);
				y = _wtoi(str.c_str());
				GetWindowText(hwndElem, text, 30);

				_panel->Read(panel);
				if (wcscmp(text, L"Dot (Intersection)") == 0 ||
					wcscmp(text, L"Start") == 0 || wcscmp(text, L"Exit") == 0)
					_panel->ClearGridSymbol(x * 2, y * 2);
				else if (wcscmp(text, L"Gap (Column)") == 0 ||
					wcscmp(text, L"Dot (Column)") == 0)
					_panel->ClearGridSymbol(x * 2, y * 2 + 1);
				else if (wcscmp(text, L"Gap (Row)") == 0 ||
					wcscmp(text, L"Dot (Row)") == 0)
					_panel->ClearGridSymbol(x * 2 + 1, y * 2);
				else 
					_panel->ClearSymbol(x, y);
				_panel->Write(panel);
				break;

			case IDC_TEST:
				//_panel->Write(panel);
				srand(static_cast<unsigned int>(time(NULL)));
				//srand(ctr++);
				//srand(1);

				//generator->setGridSize(4, 4);
				generator->generate(0x04CA4, Decoration::Dot_Intersection, 25, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2);

				//generator->setGridSize(6, 6);
				//generator->generate(panel, Decoration::Stone | Decoration::Color::White, 8, Decoration::Stone | Decoration::Color::Black, 11);
				//generator->generate(panel, Decoration::Stone | Decoration::Color::White, 5, Decoration::Stone | Decoration::Color::Black, 7);
				//generator->generate(panel, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 7, Decoration::Gap, 4);
				//generator->generate(panel, Decoration::Start, 3, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 6);
				//generator->generate(panel, Decoration::Star | Decoration::Color::White, 4, Decoration::Star | Decoration::Color::Black, 4);
				//generator->generate(panel, Decoration::Stone | Decoration::Color::White, 3, Decoration::Stone | Decoration::Color::Black, 3,
				//						   Decoration::Star | Decoration::Color::White, 3, Decoration::Star | Decoration::Color::Black, 3);
				//generator->generate(panel, Decoration::Triangle | Decoration::Color::Orange, 8, Decoration::Start, 3, Decoration::Exit, 3 );
				//generator->generate(panel, Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Star | Decoration::Color::Orange, 3);
				//generator->generate(panel, Decoration::Shape::Poly | Decoration::Can_Rotate | Decoration::Color::Green, 3);
				//generator->generate(panel, Decoration::Shape::Poly | Decoration::Can_Rotate | Decoration::Color::Yellow, 1,
				//	Decoration::Shape::Poly | Decoration::Color::Yellow, 2);
				//generator->generate(panel, Decoration::Shape::Poly | Decoration::Color::Yellow, 3,
				//						   Decoration::Shape::Poly | Decoration::Negative | Decoration::Color::Blue, 2);
				//generator->generate(panel, Decoration::Dot, 10);
				//generator->generate(panel, Decoration::Dot, 4, Decoration::Dot | Decoration::Blue, 1, Decoration::Dot | Decoration::Yellow, 2,
				//							Decoration::Eraser | Decoration::Color::White, 1);
				//generator->generate(panel, Decoration::Dot | Decoration::Blue, 1, Decoration::Dot | Decoration::Yellow, 2,
				//							Decoration::Star | Decoration::Color::Cyan, 4, Decoration::Star | Decoration::Color::Yellow, 4);
				//generator->generate(panel, Decoration::Stone | Decoration::Color::White, 5, Decoration::Stone | Decoration::Color::Black, 7,
				//							Decoration::Dot, 10);
				//generator->generateMaze(panel);

				//generator->generate(panel, Decoration::Stone | Decoration::Color::White, 6, Decoration::Stone | Decoration::Color::Black, 7,
				//						   Decoration::Eraser | Decoration::Color::White, 1);
				//generator->generate(panel, Decoration::Dot, 12, Decoration::Eraser | Decoration::Color::White, 1);
				//generator->generate(panel, Decoration::Stone | Decoration::Color::White, 5, Decoration::Stone | Decoration::Color::Black, 7,
				//							Decoration::Dot, 10, Decoration::Eraser | Decoration::Color::White, 1);
				//generator->generate(panel, Decoration::Star | Decoration::Color::Orange, 5, Decoration::Star | Decoration::Color::Magenta, 6,
				//							Decoration::Eraser | Decoration::Color::Orange, 1);
				//generator->generate(panel, Decoration::Stone | Decoration::Color::White, 3, Decoration::Stone | Decoration::Color::Black, 3,
				//						   Decoration::Star | Decoration::Color::White, 3, Decoration::Star | Decoration::Color::Black, 3,
				//							Decoration::Eraser | Decoration::Color::White, 1 );
				//generator->generate(panel, Decoration::Triangle | Decoration::Color::Orange, 8, Decoration::Eraser | Decoration::Color::White, 1);
				//generator->generate(panel, Decoration::Shape::Poly | Decoration::Can_Rotate | Decoration::Color::Yellow, 1,
				//							Decoration::Shape::Poly | Decoration::Color::Yellow, 2, Decoration::Eraser | Decoration::Color::White, 1);
				//generator->generate(panel, Decoration::Shape::Poly | Decoration::Color::Yellow, 3,
				//						   Decoration::Shape::Poly | Decoration::Negative | Decoration::Color::Blue, 2,
				//							Decoration::Eraser | Decoration::Color::White, 1);
				break;

			case IDC_ROTATED:
				CheckDlgButton(hwnd, IDC_ROTATED, !IsDlgButtonChecked(hwnd, IDC_ROTATED));
				break;

			case IDC_NEGATIVE:
				CheckDlgButton(hwnd, IDC_NEGATIVE, !IsDlgButtonChecked(hwnd, IDC_NEGATIVE));
				break;

			case IDC_SYMMETRYX:
				CheckDlgButton(hwnd, IDC_SYMMETRYX, !IsDlgButtonChecked(hwnd, IDC_SYMMETRYX));
				if (IsDlgButtonChecked(hwnd, IDC_SYMMETRYX) && IsDlgButtonChecked(hwnd, IDC_SYMMETRYY))
					_panel->symmetry = Panel::Symmetry::Rotational;
				else if (IsDlgButtonChecked(hwnd, IDC_SYMMETRYX))
					_panel->symmetry = Panel::Symmetry::Horizontal;
				else if (IsDlgButtonChecked(hwnd, IDC_SYMMETRYY))
					_panel->symmetry = Panel::Symmetry::Vertical;
				else _panel->symmetry = Panel::Symmetry::None;
				_panel->Write(panel);
				break;

			case IDC_SYMMETRYY:
				CheckDlgButton(hwnd, IDC_SYMMETRYY, !IsDlgButtonChecked(hwnd, IDC_SYMMETRYY));
				if (IsDlgButtonChecked(hwnd, IDC_SYMMETRYX) && IsDlgButtonChecked(hwnd, IDC_SYMMETRYY))
					_panel->symmetry = Panel::Symmetry::Rotational;
				else if (IsDlgButtonChecked(hwnd, IDC_SYMMETRYX))
					_panel->symmetry = Panel::Symmetry::Horizontal;
				else if (IsDlgButtonChecked(hwnd, IDC_SYMMETRYY))
					_panel->symmetry = Panel::Symmetry::Vertical;
				else _panel->symmetry = Panel::Symmetry::None;
				_panel->Write(panel);
				break;

			case SHAPE_11: CheckDlgButton(hwnd, SHAPE_11, !IsDlgButtonChecked(hwnd, SHAPE_11)); currentShape ^= SHAPE_11; break;
			case SHAPE_12: CheckDlgButton(hwnd, SHAPE_12, !IsDlgButtonChecked(hwnd, SHAPE_12)); currentShape ^= SHAPE_12; break;
			case SHAPE_13: CheckDlgButton(hwnd, SHAPE_13, !IsDlgButtonChecked(hwnd, SHAPE_13)); currentShape ^= SHAPE_13; break;
			case SHAPE_14: CheckDlgButton(hwnd, SHAPE_14, !IsDlgButtonChecked(hwnd, SHAPE_14)); currentShape ^= SHAPE_14; break;
			case SHAPE_21: CheckDlgButton(hwnd, SHAPE_21, !IsDlgButtonChecked(hwnd, SHAPE_21)); currentShape ^= SHAPE_21; break;
			case SHAPE_22: CheckDlgButton(hwnd, SHAPE_22, !IsDlgButtonChecked(hwnd, SHAPE_22)); currentShape ^= SHAPE_22; break;
			case SHAPE_23: CheckDlgButton(hwnd, SHAPE_23, !IsDlgButtonChecked(hwnd, SHAPE_23)); currentShape ^= SHAPE_23; break;
			case SHAPE_24: CheckDlgButton(hwnd, SHAPE_24, !IsDlgButtonChecked(hwnd, SHAPE_24)); currentShape ^= SHAPE_24; break;
			case SHAPE_31: CheckDlgButton(hwnd, SHAPE_31, !IsDlgButtonChecked(hwnd, SHAPE_31)); currentShape ^= SHAPE_31; break;
			case SHAPE_32: CheckDlgButton(hwnd, SHAPE_32, !IsDlgButtonChecked(hwnd, SHAPE_32)); currentShape ^= SHAPE_32; break;
			case SHAPE_33: CheckDlgButton(hwnd, SHAPE_33, !IsDlgButtonChecked(hwnd, SHAPE_33)); currentShape ^= SHAPE_33; break;
			case SHAPE_34: CheckDlgButton(hwnd, SHAPE_34, !IsDlgButtonChecked(hwnd, SHAPE_34)); currentShape ^= SHAPE_34; break;
			case SHAPE_41: CheckDlgButton(hwnd, SHAPE_41, !IsDlgButtonChecked(hwnd, SHAPE_41)); currentShape ^= SHAPE_41; break;
			case SHAPE_42: CheckDlgButton(hwnd, SHAPE_42, !IsDlgButtonChecked(hwnd, SHAPE_42)); currentShape ^= SHAPE_42; break;
			case SHAPE_43: CheckDlgButton(hwnd, SHAPE_43, !IsDlgButtonChecked(hwnd, SHAPE_43)); currentShape ^= SHAPE_43; break;
			case SHAPE_44: CheckDlgButton(hwnd, SHAPE_44, !IsDlgButtonChecked(hwnd, SHAPE_44)); currentShape ^= SHAPE_44; break;
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

	hwndLoadingText = CreateWindow(L"STATIC", L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		135, 50, 160, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"STATIC", L"Col/Row:",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		160, 130, 100, 26, hwnd, NULL, hInstance, NULL);
	hwndCol = CreateWindow(MSFTEDIT_CLASS, L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
		220, 130, 50, 26, hwnd, NULL, hInstance, NULL);
	hwndRow = CreateWindow(MSFTEDIT_CLASS, L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
		270, 130, 50, 26, hwnd, NULL, hInstance, NULL);
	SetWindowText(hwndCol, L"0");
	SetWindowText(hwndRow, L"0");
	
	hwndElem = CreateWindow(L"COMBOBOX", L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_HASSTRINGS,
		160, 160, 150, 300, hwnd, NULL, hInstance, NULL);

	const int NUM_ELEMS = 14;
	TCHAR elems[NUM_ELEMS][24] =
	{
		TEXT("Start"), TEXT("Exit"), TEXT("Gap (Row)"), TEXT("Gap (Column)"),
		TEXT("Dot (Intersection)"), TEXT("Dot (Row)"), TEXT("Dot (Column)"),
		TEXT("Stone"), TEXT("Star"), TEXT("Eraser"), TEXT("Shape"),
		TEXT("Triangle 1"), TEXT("Triangle 2"), TEXT("Triangle 3")
	};
	TCHAR option[24];
	memset(&option, 0, sizeof(option));
	for (int i = 0; i < NUM_ELEMS; i++)
	{
		wcscpy_s(option, sizeof(option) / sizeof(TCHAR), (TCHAR*)elems[i]);
		SendMessage(hwndElem, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)option);
	}
	SendMessage(hwndElem, CB_SETCURSEL, (WPARAM)7, (LPARAM)0);

	hwndColor = CreateWindow(L"COMBOBOX", L"",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWN | CBS_HASSTRINGS,
		160, 190, 150, 300, hwnd, NULL, hInstance, NULL);

	const int NUM_COLORS = 12;
	TCHAR colors[NUM_COLORS][24] =
	{
		TEXT("Black"), TEXT("White"), TEXT("Red"), TEXT("Orange"), TEXT("Yellow"), TEXT("Green"),
		TEXT("Cyan"), TEXT("Blue"), TEXT("Purple"), TEXT("Magenta"), TEXT("None"), TEXT("???")
	};
	memset(&option, 0, sizeof(option));
	for (int i = 0; i < NUM_COLORS; i++)
	{
		wcscpy_s(option, sizeof(option) / sizeof(TCHAR), (TCHAR*)colors[i]);
		SendMessage(hwndColor, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)option);
	}
	SendMessage(hwndColor, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	CreateWindow(L"BUTTON", L"Place Symbol",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 220, 150, 26, hwnd, (HMENU)IDC_ADD, hInstance, NULL);
	CreateWindow(L"BUTTON", L"Remove Symbol",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 250, 150, 26, hwnd, (HMENU)IDC_REMOVE, hInstance, NULL);
	CreateWindow(L"BUTTON", L"Test",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		160, 330, 150, 26, hwnd, (HMENU)IDC_TEST, hInstance, NULL);

	CreateWindow(L"STATIC", L"Shape:",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		50, 150, 50, 16, hwnd, NULL, hInstance, NULL);

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			CreateWindow(L"BUTTON", L"",
				WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
				50 + x * 15, 180 + y * 15, 12, 12, hwnd, (HMENU)shapePos[x + y * 4], hInstance, NULL);
		}
	}
	for (long long chk : defaultShape) {
		CheckDlgButton(hwnd, static_cast<int>(chk), TRUE);
		currentShape |= chk;
	}

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		50, 260, 12, 12, hwnd, (HMENU)IDC_ROTATED, hInstance, NULL);
	CreateWindow(L"STATIC", L"Rotated",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		65, 260, 80, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		50, 280, 12, 12, hwnd, (HMENU)IDC_NEGATIVE, hInstance, NULL);
	CreateWindow(L"STATIC", L"Negative",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		65, 280, 80, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		50, 320, 12, 12, hwnd, (HMENU)IDC_SYMMETRYX, hInstance, NULL);
	CreateWindow(L"STATIC", L"H. Symmetry",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		65, 320, 90, 16, hwnd, NULL, hInstance, NULL);

	CreateWindow(L"BUTTON", L"",
		WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
		50, 340, 12, 12, hwnd, (HMENU)IDC_SYMMETRYY, hInstance, NULL);
	CreateWindow(L"STATIC", L"V. Symmetry",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | SS_LEFT,
		65, 340, 90, 16, hwnd, NULL, hInstance, NULL);

	_panel->Read(panel);
	//generator->initPanel(_panel);

	if (_panel->symmetry == Panel::Symmetry::Horizontal || _panel->symmetry == Panel::Symmetry::Rotational)
		CheckDlgButton(hwnd, IDC_SYMMETRYX, TRUE);
	if (_panel->symmetry == Panel::Symmetry::Vertical|| _panel->symmetry == Panel::Symmetry::Rotational)
		CheckDlgButton(hwnd, IDC_SYMMETRYY, TRUE);

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
