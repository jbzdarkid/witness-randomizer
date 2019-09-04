#include "PuzzleList.h"

void PuzzleList::GenerateAllN()
{
	generator->setLoadingData(143);
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
	generator->config |= Generate::Config::FullGaps;
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
	generator->config |= Generate::Config::FullGaps;
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
	generator->config |= Generate::Config::StartEdgeOnly;
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
	generator->config |= Generate::Config::FullGaps;
	generator->generate(0x000B0, Decoration::Dot, 8, Decoration::Gap, 5);
	//Black Dots
	generator->resetConfig();
	generator->setGridSize(5, 5);
	generator->config |= Generate::Config::StartEdgeOnly;
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
	generator->setLoadingData(L"Quarry", 39);
	//Entry Gates
	generator->resetConfig();
	generator->setGridSize(5, 5);
	generator->generate(0x09E57, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5, Decoration::Gap, 10);
	generator->setGridSize(4, 4);
	generator->config |= Generate::Config::RequireCombineShapes;
	generator->generate(0x17C09, Decoration::Poly, 2, Decoration::Gap, 5);
	//Mill Entry Door
	generator->resetConfig();
	generator->generate(0x01E5A, Decoration::Stone | Decoration::Color::White, 5, Decoration::Stone | Decoration::Color::Black, 7);
	generator->config |= Generate::Config::PreserveStructure;
	generator->generate(0x01E59, Decoration::Dot, 15);
	//Dots
	generator->resetConfig();
	generator->setGridSize(3, 3);
	generator->generate(0x00E0C, Decoration::Dot, 5, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x01489, Decoration::Dot, 7, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->setGridSize(4, 3);
	generator->generate(0x0148A, Decoration::Dot, 11, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x014D9, Decoration::Dot, 13, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->setGridSize(4, 4);
	generator->generate(0x014E7, Decoration::Dot, 15, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x014E8, Decoration::Dot, 15, Decoration::Eraser | Decoration::Color::Green, 1);
	//Stones
	//The colors here don't correspond because of "Push Colors" flag. I don't reset the colors because I want to be able to use red stones
	//TODO: Figure out how to stop panels' backgrounds from getting screwed up
	generator->config |= Generate::Config::MakeStonesUnsolvable;
	generator->setGridSize(3, 3);
	generator->generate(0x00557, Decoration::Stone | Decoration::Color::White, 3, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x005F1, Decoration::Stone | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->setGridSize(4, 4);
	generator->generate(0x00620, Decoration::Stone | Decoration::Color::White, 7, Decoration::Stone | Decoration::Color::Black, 6, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x009F5, Decoration::Stone | Decoration::Color::White, 6, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x0146C, Decoration::Stone | Decoration::Color::White, 7, Decoration::Stone | Decoration::Color::Black, 6, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->resetConfig();
	generator->setGridSize(3, 3);
	generator->generate(0x3C12D, Decoration::Stone | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::Red, 2, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->setGridSize(4, 4);
	generator->generate(0x03686, Decoration::Stone | Decoration::Color::Red, 4, Decoration::Stone | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x014E9, Decoration::Stone | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::Red, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	//Dots + Stones
	generator->resetConfig();
	generator->generate(0x0367C, Decoration::Stone | Decoration::Color::White, 5, Decoration::Stone | Decoration::Color::Black, 5, Decoration::Dot, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x3C125, Decoration::Stone | Decoration::Color::White, 10, Decoration::Stone | Decoration::Color::Black, 10,
		Decoration::Dot, 10, Decoration::Eraser | Decoration::Color::Purple, 1, Decoration::Start, 3);
	//Boathouse Ramp Activation
	generator->generate(0x034D4, Decoration::Star | Decoration::White, 10, Decoration::Star | Decoration::Black, 6);
	generator->config |= Generate::Config::RequireCombineShapes;
	generator->generate(0x021D5, Decoration::Poly, 1, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Gap, 5);
	//Eraser + Shapes
	generator->resetConfig();
	generator->generate(0x021B3, Decoration::Poly, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x021B4, Decoration::Poly, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x021B0, Decoration::Poly, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x021AF, Decoration::Poly, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->config |= Generate::Config::FullAreaEraser;
	generator->generate(0x021AF, Decoration::Poly, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->resetConfig();
	generator->config |= Generate::Config::DisconnectShapes;
	generator->generate(0x021AE, Decoration::Poly, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	//Eraser + Stars
	//I need to reset colors to get orange, but I don't reset immediately because it makes the green stars look bad
	generator->resetConfig(); 
	generator->generate(0x021B5, Decoration::Star | Decoration::Color::Black, 3, Decoration::Star | Decoration::Color::White, 4, Decoration::Eraser | Decoration::Color::White, 1, Decoration::Gap, 2);
	generator->generate(0x021B6, Decoration::Star | Decoration::Color::Black, 3, Decoration::Star | Decoration::Color::White, 4, Decoration::Eraser | Decoration::Color::Black, 1, Decoration::Gap, 2);
	generator->config |= Generate::Config::ResetColors;
	generator->generate(0x021B7, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Star | Decoration::Color::Orange, 5, Decoration::Eraser | Decoration::Orange, 1);
	generator->generate(0x021BB, Decoration::Star | Decoration::Color::Magenta, 7, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Eraser | Decoration::Orange, 1);
	// TODO 0x09DB5 (Special case)
	generator->generate(0x09DB1, Decoration::Star | Decoration::Color::Orange, 9, Decoration::Eraser | Decoration::Orange, 1);
	generator->generate(0x3C124, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Star | Decoration::Color::Magenta, 5, Decoration::Star | Decoration::Color::White, 4, Decoration::Eraser | Decoration::Magenta, 1);
	//Eraser + Stars + Shapes
	generator->generate(0x09DB3, Decoration::Star | Decoration::Color::Magenta, 5, Decoration::Poly | Decoration::Color::Orange, 2, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x09DB4, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Poly | Decoration::Color::Orange, 2, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x0A3CB, Decoration::Star | Decoration::Color::Magenta, 3, Decoration::Poly | Decoration::Color::Orange, 3, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x0A3CC, Decoration::Star | Decoration::Color::Magenta, 5, Decoration::Poly | Decoration::Color::Orange, 3, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x0A3D0, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Poly | Decoration::Color::Orange, 3, Decoration::Eraser | Decoration::White, 1);
}

void PuzzleList::GenerateBunkerN()
{
}

void PuzzleList::GenerateSwampN()
{
	generator->setLoadingData(L"Swamp", 50);
	//First row
	generator->resetConfig();
	generator->config |= Generate::Config::DisableCombineShapes;
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
	generator->generate(0x00469, Decoration::Poly, 1, Decoration::Gap, 5);
	generator->generate(0x00472, Decoration::Poly, 1, Decoration::Gap, 5);
	generator->generate(0x00262, Decoration::Poly, 1, Decoration::Gap, 5);
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x00474, Decoration::Poly, 2, Decoration::Gap, 8);
	generator->generate(0x00553, Decoration::Poly, 2, Decoration::Gap, 8);
	generator->setGridSize(5, 5);
	generator->pathWidth = 0.6f;
	generator->generate(0x0056F, Decoration::Poly, 2, Decoration::Gap, 12);
	//Second Row
	generator->resetConfig();
	generator->config |= Generate::Config::RequireCombineShapes;
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
	generator->generate(0x00390, Decoration::Poly, 2, Decoration::Gap, 3);
	generator->generate(0x010CA, Decoration::Poly, 2, Decoration::Gap, 3);
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x00983, Decoration::Poly, 2, Decoration::Gap, 5);
	generator->generate(0x00984, Decoration::Poly, 2, Decoration::Gap, 5);
	generator->generate(0x00986, Decoration::Poly, 2, Decoration::Gap, 5);
	generator->setGridSize(5, 5);
	generator->generate(0x00985, Decoration::Poly, 2, Decoration::Gap, 8);
	generator->generate(0x00987, Decoration::Poly, 2, Decoration::Gap, 8);
	generator->generate(0x181A9, Decoration::Poly, 3, Decoration::Gap, 8);
	//Turn off bridge control until all previous puzzles are solved
	specialCase->deactivateAndTarget(0x00609, 0x181A9);
	//Red Panels
	generator->resetConfig();
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x00982, Decoration::Poly, 3, Decoration::Gap, 2);
	generator->generate(0x0097F, Decoration::Poly, 3, Decoration::Gap, 2);
	generator->setGridSize(5, 5);
	generator->pathWidth = 0.6f;
	generator->generate(0x0098F, Decoration::Poly, 4, Decoration::Gap, 2);
	generator->generate(0x00990, Decoration::Poly, 4, Decoration::Gap, 2);
	//TODO: Replace these shortcut panels with hand-made alternate solution puzzles
	generator->generate(0x17C0D, Decoration::Poly, 3);
	generator->generate(0x17C0E, Decoration::Poly, 3, Decoration::Gap, 6);
	//Disconnected Shapes
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
	generator->config |= Generate::Config::DisconnectShapes;
	generator->generate(0x00999, Decoration::Poly, 2);
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x0099D, Decoration::Poly, 3);
	generator->generate(0x009A0, Decoration::Poly, 3);
	generator->generate(0x009A1, Decoration::Poly, 3);
	//Rotating Shapes
	generator->resetConfig();
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
	generator->generate(0x00007, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Gap, 3);
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x00008, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Gap, 6);
	generator->generate(0x00009, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Gap, 6);
	generator->generate(0x0000A, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Gap, 6);
	generator->setGridSize(5, 5);
	generator->pathWidth = 0.6f;
	generator->generate(0x0000A, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Gap, 12);
	//5x5 Rotating Shapes
	generator->generate(0x003B2, Decoration::Poly | Decoration::Can_Rotate, 3, Decoration::Gap, 4);
	generator->generate(0x00A1E, Decoration::Poly | Decoration::Can_Rotate, 4);
	generator->generate(0x00C2E, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Poly, 2, Decoration::Gap, 3);
	generator->config |= Generate::Config::DisconnectShapes;
	generator->generate(0x00E3A, Decoration::Poly | Decoration::Can_Rotate, 4);
	//Optional Tetris
	generator->resetConfig();
	generator->generate(0x009A6, Decoration::Poly, 6);
	//Negative Shapes 1
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
	generator->config |= Generate::Config::DisableCancelShapes;
	generator->generate(0x009AB, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 1);
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x009AD, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 1);
	generator->generate(0x009AE, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 1);
	generator->generate(0x009AF, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 1);
	generator->config |= Generate::Config::DisconnectShapes;
	generator->generate(0x00006, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 1);
	//Negative Shapes 2
	generator->resetConfig();
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->config |= Generate::Config::DisableCancelShapes;
	generator->generate(0x00002, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x00004, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x00005, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x013E6, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x00596, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 3);
	//Negative Shapes 3
	generator->resetConfig();
	generator->config |= Generate::Config::RequireCancelShapes;
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
	generator->generate(0x00001, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 2);
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x014D2, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x014D4, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 3);
	generator->generate(0x014D1, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 4);
	//Exit Shortcut
	generator->resetConfig();
	generator->generate(0x17C05, Decoration::Poly | Decoration::Can_Rotate, 2);
	specialCase->generateAntiPuzzle(0x17C02);
}

void PuzzleList::GenerateTreehouseN()
{
	generator->resetConfig();
	generator->generate(0x02886, Decoration::Star | Decoration::Color::Orange, 2, Decoration::Gap, 5);
	//Yellow Bridge

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
