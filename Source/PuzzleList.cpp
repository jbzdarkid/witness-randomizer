#include "PuzzleList.h"

void PuzzleList::GenerateAllN()
{
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
	generator->setLoadingData(L"Tutorial", 19);
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
	generator->setFullGaps = false;
	//2 starts maze
	generator->setGridSize(0, 0);
	generator->generateMaze(0x0A3B2);
	//2 exits maze
	generator->generateMaze(0x0A3B5);
	//Secret back area
	//TODO: Make full dot puzzles work
	//gen->generate(0x0A171, Decoration::Dot_Intersection, 16, Decoration::Gap, 3);
	//gen->generate(0x04CA4, Decoration::Dot_Intersection, 16, Decoration::Gap, 4);
	//Dots Tutorial
	generator->setFullGaps = true;
	generator->setGridSize(3, 3);
	generator->generate(0x0005D, Decoration::Start, 1, Decoration::Exit, 1, Decoration::Dot_Intersection, 7, Decoration::Gap, 4);
	generator->generate(0x0005E, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 7, Decoration::Gap, 4);
	generator->setGridSize(4, 4);
	generator->generate(0x0005F, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->generate(0x00060, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->generate(0x00061, Decoration::Start, 3, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->setFullGaps = false;
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
