#include "PuzzleList.h"

void PuzzleList::GenerateAllN()
{
	generator->setLoadingData(54);
	GenerateTutorialN();
	GenerateSymmetryN();
	GenerateQuarryN();
	GenerateBunkerN();
	GenerateSwampN();
	GenerateTreehouseN();
	GenerateTownN();
	GenerateVaultsN();
	GenerateTrianglePanelsN();
	GenerateMountainN();
	GenerateOrchardN();
	GenerateDesertN();
	//GenerateShadowsN(); //Can't randomize
	GenerateKeepN();
	//GenerateMonasteryN(); //Can't randomize
	GenerateJungleN();
	SetWindowText(_handle, L"Done!");
}

void PuzzleList::GenerateAllH()
{
	GenerateTutorialH();
	GenerateSymmetryH();
	GenerateQuarryH();
	GenerateBunkerH();
	GenerateSwampH();
	GenerateTreehouseH();
	GenerateTownH();
	GenerateVaultsH();
	GenerateTrianglePanelsH();
	GenerateMountainH();
	GenerateOrchardH();
	GenerateDesertH();
	//GenerateShadowsH(); //Can't randomize
	GenerateKeepH();
	//GenerateMonasteryH(); //Can't randomize
	GenerateJungleH();
}

void PuzzleList::GenerateTutorialN()
{
	generator->setLoadingData(L"Tutorial", 21);
	//TODO: Display a message on this panel								0x00064, // Tutorial Straight
	//TODO: Display a message on this panel								0x00182, // Tutorial Bend
	//Mazes
	generator->setFullGaps = true;
	generator->setGridSize(6, 6);
	generator->generateMaze(0x00293);
	generator->setGridSize(9, 9);
	generator->generateMaze(0x00295, 1, 1);
	generator->setGridSize(12, 12);
	generator->generateMaze(0x002C2);
	generator->resetConfig();
	//2 starts maze
	generator->generateMaze(0x0A3B2);
	//2 exits maze
	generator->generateMaze(0x0A3B5);
	//Secret back area
	generator->generate(0x0A171, Decoration::Dot_Intersection, 25, Decoration::Gap, 4);
	generator->generate(0x04CA4, Decoration::Dot_Intersection, 25, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2);
	//Dots Tutorial
	generator->setFullGaps = true;
	generator->setGridSize(3, 3);
	generator->generate(0x0005D, Decoration::Start, 1, Decoration::Exit, 1, Decoration::Dot_Intersection, 7, Decoration::Gap, 4);
	generator->generate(0x0005E, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 7, Decoration::Gap, 4);
	generator->setGridSize(4, 4);
	generator->generate(0x0005F, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->generate(0x00060, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->generate(0x00061, Decoration::Start, 3, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->resetConfig();
	//Stones Tutorial
	generator->setGridSize(4, 4);
	generator->generate(0x018AF, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5);
	generator->generate(0x0001B, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5);
	generator->generate(0x012C9, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5, Decoration::Start, 3);
	generator->setGridSize(5, 5);
	generator->generate(0x0001C, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 11, Decoration::Stone | Decoration::Color::White, 8);
	generator->generate(0x0001D, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 11, Decoration::Stone | Decoration::Color::White, 8);
	generator->generate(0x0001E, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 11, Decoration::Stone | Decoration::Color::White, 8, Decoration::Start, 3);
	generator->generate(0x0001F, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5, Decoration::Gap, 10);
	generator->generate(0x00020, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5, Decoration::Gap, 10);
	generator->generate(0x00021, Decoration::Exit, 1, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5, Decoration::Gap, 10, Decoration::Start, 3);
}

void PuzzleList::GenerateSymmetryN()
{
	generator->setLoadingData(L"Symmetry", 33);
	//Vertical Symmetry Mazes
	generator->setGridSize(5, 5);
	generator->centralStart = false;
	generator->generateMaze(0x00086, 0, 1);
	generator->generateMaze(0x00087, 1, 1);
	generator->resetConfig();
	generator->generateMaze(0x00059, 1, 0);
	generator->generateMaze(0x00062);
	generator->generateMaze(0x0005C);
	//Rotational Symmetry Mazes
	generator->setGridSize(5, 5);
	generator->generateMaze(0x0008D, 0, 1);
	generator->generateMaze(0x00081, 1, 1);
	generator->setGridSize(6, 6);
	generator->generateMaze(0x00083);
	generator->setGridSize(7, 7);
	generator->pathWidth = 0.8f;
	generator->generateMaze(0x00084);
	generator->generateMaze(0x00082);
	generator->generateMaze(0x0343A);
	//Symmetry Island Door
	generator->resetConfig();
	generator->setGridSize(4, 4);
	generator->setFullGaps = true;
	generator->generate(0x000B0, Decoration::Dot, 8, Decoration::Gap, 5);
	//Black Dots
	generator->resetConfig();
	generator->setGridSize(5, 5);
	generator->centralStart = false;
	generator->setSymmetry(Panel::Symmetry::Horizontal);
	generator->generate(0x00022, Decoration::Dot, 10, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x00023, Decoration::Dot, 10, Decoration::Start, 1, Decoration::Exit, 1);
	generator->setSymmetry(Panel::Symmetry::Rotational);
	generator->generate(0x00024, Decoration::Dot, 10, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x00025, Decoration::Dot, 10, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x00026, Decoration::Dot, 10, Decoration::Start, 1, Decoration::Exit, 1);
	//Colored Dots
	generator->generate(0x0007C, Decoration::Dot | Decoration::Color::Blue, 3, Decoration::Dot | Decoration::Color::Yellow, 3, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x0007E, Decoration::Dot | Decoration::Color::Blue, 3, Decoration::Dot | Decoration::Color::Yellow, 3, Decoration::Start, 1, Decoration::Exit, 2);
	generator->generate(0x00075, Decoration::Dot | Decoration::Color::Blue, 2, Decoration::Dot | Decoration::Color::Yellow, 2, Decoration::Dot, 3, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x00073, Decoration::Dot | Decoration::Color::Blue, 1, Decoration::Dot | Decoration::Color::Yellow, 2, Decoration::Dot, 4, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x00077, Decoration::Dot | Decoration::Color::Blue, 1, Decoration::Dot | Decoration::Color::Yellow, 2, Decoration::Dot, 4, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x00079, Decoration::Dot | Decoration::Color::Blue, 2, Decoration::Dot | Decoration::Color::Yellow, 2, Decoration::Dot, 4, Decoration::Start, 2, Decoration::Exit, 1);
	//Fading Lines
	generator->setGridSize(6, 6);
	generator->generate(0x00065, Decoration::Dot | Decoration::Color::Blue, 4, Decoration::Dot | Decoration::Color::Yellow, 2, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x0006D, Decoration::Dot | Decoration::Color::Blue, 3, Decoration::Dot | Decoration::Color::Yellow, 3, Decoration::Start, 1, Decoration::Exit, 2);
	generator->generate(0x00072, Decoration::Dot | Decoration::Color::Blue, 2, Decoration::Dot | Decoration::Color::Yellow, 4, Decoration::Start, 2, Decoration::Exit, 1);
	generator->setSymbol(Decoration::Start, 0, 6);  generator->setSymbol(Decoration::Start, 12, 6);
	generator->generate(0x0006F, Decoration::Dot | Decoration::Color::Blue, 3, Decoration::Dot | Decoration::Color::Yellow, 3, Decoration::Exit, 1);
	generator->setSymmetry(Panel::Symmetry::Vertical);
	generator->setSymbol(Decoration::Start, 0, 6); generator->setSymbol(Decoration::Start, 12, 6);
	generator->generate(0x00070, Decoration::Dot | Decoration::Color::Blue, 3, Decoration::Dot | Decoration::Color::Yellow, 3, Decoration::Exit, 1);
	generator->setSymmetry(Panel::Symmetry::Rotational);
	generator->setSymbol(Decoration::Start, 0, 6); generator->setSymbol(Decoration::Start, 12, 6);
	generator->generate(0x00071, Decoration::Dot | Decoration::Color::Blue, 2, Decoration::Dot | Decoration::Color::Yellow, 3, Decoration::Start, 1, Decoration::Exit, 1);
	generator->generate(0x00076, Decoration::Dot | Decoration::Color::Yellow, 5, Decoration::Start, 1, Decoration::Exit, 1);
	//Symmetry Island Door 2 not randomized until I can ensure solution passes through center
	//Dot Reflection Dual Panels (before laser)
	generator->resetConfig();
	std::set<Panel::Symmetry> normalSym = { Panel::Symmetry::Horizontal, Panel::Symmetry::Vertical, Panel::Symmetry::Rotational };
	std::set<Panel::Symmetry> weirdSym = { Panel::Symmetry::RotateLeft, Panel::Symmetry::RotateRight, Panel::Symmetry::FlipXY, Panel::Symmetry::FlipNegXY };
	specialCase->generateReflectionDotPuzzle(0x00A52, 0x00A61, { std::make_pair<int,int>(Decoration::Dot, 10),
		std::make_pair<int,int>(Decoration::Exit, 1), std::make_pair<int,int>(Decoration::Gap, 5) }, pop_random(normalSym) );
	generator->setSymbol(Decoration::Start, 0, 8);
	specialCase->generateReflectionDotPuzzle(0x00A57, 0x00A64, { std::make_pair<int,int>(Decoration::Dot, 12),
		std::make_pair<int,int>(Decoration::Exit, 1), std::make_pair<int,int>(Decoration::Gap, 1) }, pop_random(normalSym) );
	generator->setSymbol(Decoration::Start, 0, 8); generator->setSymbol(Decoration::Start, 8, 8); generator->setSymbol(Decoration::Start, 8, 0); generator->setSymbol(Decoration::Start, 0, 0);
	generator->setSymbol(Decoration::Exit, 8, 2);
	specialCase->generateReflectionDotPuzzle(0x00A5B, 0x00A68, { std::make_pair<int,int>(Decoration::Dot, 12) }, pop_random(weirdSym) );
}

void PuzzleList::GenerateQuarryN()
{
}

void PuzzleList::GenerateBunkerN()
{
}

void PuzzleList::GenerateSwampN()
{
}

void PuzzleList::GenerateTreehouseN()
{
}

void PuzzleList::GenerateTownN()
{
}

void PuzzleList::GenerateVaultsN()
{
}

void PuzzleList::GenerateTrianglePanelsN()
{
}

void PuzzleList::GenerateMountainN()
{
}

void PuzzleList::GenerateCavesN()
{
}

void PuzzleList::GenerateOrchardN()
{
}

void PuzzleList::GenerateDesertN()
{
}

void PuzzleList::GenerateKeepN()
{
}

void PuzzleList::GenerateJungleN()
{
}

void PuzzleList::GenerateTutorialH()
{
}

void PuzzleList::GenerateSymmetryH()
{
}

void PuzzleList::GenerateQuarryH()
{
}

void PuzzleList::GenerateBunkerH()
{
}

void PuzzleList::GenerateSwampH()
{
}

void PuzzleList::GenerateTreehouseH()
{
}

void PuzzleList::GenerateTownH()
{
}

void PuzzleList::GenerateVaultsH()
{
}

void PuzzleList::GenerateTrianglePanelsH()
{
}

void PuzzleList::GenerateMountainH()
{
}

void PuzzleList::GenerateCavesH()
{
}

void PuzzleList::GenerateOrchardH()
{
}

void PuzzleList::GenerateDesertH()
{
}

void PuzzleList::GenerateKeepH()
{
}

void PuzzleList::GenerateJungleH()
{
}
