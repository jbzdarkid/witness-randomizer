#include "PuzzleList.h"

void PuzzleList::GenerateAllN()
{
	generator->setLoadingData(328);
	GenerateTutorialN();
	GenerateSymmetryN();
	GenerateQuarryN();
	//GenerateBunkerN(); //Can't randomize because panels refuse to render the symbols
	GenerateSwampN();
	GenerateTreehouseN();
	GenerateTownN();
	GenerateVaultsN();
	GenerateTrianglePanelsN();
	GenerateOrchardN();
	GenerateKeepN();
	GenerateJungleN();
	GenerateMountainN();
	GenerateCavesN();
	SetWindowText(_handle, L"Done!");
	//GenerateDesertN();
	//GenerateShadowsN(); //Can't randomize
	//GenerateMonasteryN(); //Can't randomize
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
	GenerateOrchardH();
	GenerateKeepH();
	GenerateJungleH();
	GenerateMountainH();
	GenerateCavesH();
	SetWindowText(_handle, L"Done!");
	//GenerateDesertH();
	//GenerateShadowsH(); //Can't randomize
	//GenerateMonasteryH(); //Can't randomize
}

void PuzzleList::GenerateTutorialN()
{
	generator->setLoadingData(L"Tutorial", 21);
	generator->resetConfig();
	//TODO: Display a message on this panel								0x00064, // Tutorial Straight
	//TODO: Display a message on this panel								0x00182, // Tutorial Bend
	//Mazes
	generator->setFlag(Generate::Config::FullGaps);
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
	generator->setFlag(Generate::Config::FullGaps);
	generator->setGridSize(3, 3);
	generator->generate(0x0005D, Decoration::Start, 1, Decoration::Exit, 1, Decoration::Dot_Intersection, 7, Decoration::Gap, 4);
	generator->generate(0x0005E, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 7, Decoration::Gap, 4);
	generator->setGridSize(4, 4);
	generator->generate(0x0005F, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->generate(0x00060, Decoration::Start, 2, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	generator->generate(0x00061, Decoration::Start, 3, Decoration::Exit, 1, Decoration::Dot_Intersection, 10, Decoration::Gap, 8);
	//Stones Tutorial
	generator->resetConfig();
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
	generator->resetConfig();
	generator->setFlag(Generate::Config::StartEdgeOnly);
	//Vertical Symmetry Mazes
	generator->setSymmetry(Panel::Symmetry::Vertical);
	generator->setGridSize(5, 5);
	generator->generateMaze(0x00086, 0, 1);
	generator->generateMaze(0x00087, 1, 1);
	generator->setGridSize(0, 0);
	generator->removeFlagOnce(Generate::Config::StartEdgeOnly);
	generator->generateMaze(0x00059, 1, 0);
	generator->generateMaze(0x00062);
	generator->generateMaze(0x0005C);
	//Rotational Symmetry Mazes
	generator->setSymmetry(Panel::Symmetry::Rotational);
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
	generator->setFlagOnce(Generate::Config::FullGaps);
	generator->generate(0x000B0, Decoration::Dot, 8, Decoration::Gap, 5);
	//Black Dots
	generator->setFlag(Generate::Config::StartEdgeOnly);
	generator->setGridSize(5, 5);
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
	std::set<Panel::Symmetry> normalSym = { Panel::Symmetry::Horizontal, Panel::Symmetry::Rotational };
	std::set<Panel::Symmetry> weirdSym = { Panel::Symmetry::RotateLeft, Panel::Symmetry::RotateRight, Panel::Symmetry::FlipXY, Panel::Symmetry::FlipNegXY };
	specialCase->generateReflectionDotPuzzle(0x00A52, 0x00A61, { std::make_pair<int,int>(Decoration::Dot, 10),
		std::make_pair<int,int>(Decoration::Exit, 1), std::make_pair<int,int>(Decoration::Gap, 5) }, Panel::Symmetry::Vertical );
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
	generator->resetConfig();
	//Entry Gates
	generator->setGridSize(5, 5);
	generator->generate(0x09E57, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5, Decoration::Gap, 10);
	generator->setGridSize(4, 4);
	generator->setFlagOnce(Generate::Config::RequireCombineShapes);
	generator->generate(0x17C09, Decoration::Poly, 2, Decoration::Gap, 5);
	//Mill Entry Door
	generator->resetConfig();
	generator->generate(0x01E5A, Decoration::Stone | Decoration::Color::White, 5, Decoration::Stone | Decoration::Color::Black, 7);
	generator->setFlagOnce(Generate::Config::PreserveStructure);
	generator->generate(0x01E59, Decoration::Dot, 15);
	//Dots
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
	generator->setFlag(Generate::Config::MakeStonesUnsolvable);
	generator->setGridSize(3, 3);
	generator->generate(0x00557, Decoration::Stone | Decoration::Color::White, 3, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x005F1, Decoration::Stone | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->setGridSize(4, 4);
	generator->generate(0x00620, Decoration::Stone | Decoration::Color::White, 7, Decoration::Stone | Decoration::Color::Black, 6, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x009F5, Decoration::Stone | Decoration::Color::White, 6, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x0146C, Decoration::Stone | Decoration::Color::White, 7, Decoration::Stone | Decoration::Color::Black, 6, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->removeFlag(Generate::Config::MakeStonesUnsolvable);
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
	generator->setFlagOnce(Generate::Config::RequireCombineShapes);
	generator->generate(0x021D5, Decoration::Poly, 1, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Gap, 5);
	//Eraser + Shapes
	generator->generate(0x021B3, Decoration::Poly, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x021B4, Decoration::Poly, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->generate(0x021B0, Decoration::Poly, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->setFlagOnce(Generate::Config::SmallShapes);
	generator->generate(0x021AF, Decoration::Poly, 4, Decoration::Eraser | Decoration::Color::Green, 1);
	generator->setFlagOnce(Generate::Config::DisconnectShapes);
	generator->generate(0x021AE, Decoration::Poly, 3, Decoration::Eraser | Decoration::Color::Green, 1);
	//Eraser + Stars
	generator->setFlag(Generate::Config::WriteColors);
	generator->generate(0x021B5, Decoration::Star | Decoration::Color::Green, 3, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Eraser | Decoration::Color::Magenta, 1, Decoration::Gap, 2);
	generator->generate(0x021B6, Decoration::Star | Decoration::Color::Green, 3, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Eraser | Decoration::Color::Green, 1, Decoration::Gap, 2);
	generator->generate(0x021B7, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Star | Decoration::Color::Orange, 5, Decoration::Eraser | Decoration::Orange, 1);
	generator->generate(0x021BB, Decoration::Star | Decoration::Color::Magenta, 7, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Eraser | Decoration::Orange, 1);
	generator->generate(0x09DB5, { { Decoration::Star | Decoration::Color::Green, 1 },{ Decoration::Star | Decoration::Color::Magenta, 1 },{ Decoration::Star | Decoration::Color::Orange, 1 },
		{ Decoration::Eraser | Decoration::Color::Green, 1 },{ Decoration::Eraser | Decoration::Color::Magenta, 1 },{ Decoration::Eraser | Decoration::Color::Orange, 1 },
		{ Decoration::Star | Decoration::Color::White, 2 } });
	generator->generate(0x09DB1, Decoration::Star | Decoration::Color::Orange, 9, Decoration::Eraser | Decoration::Orange, 1);
	generator->generate(0x3C124, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Star | Decoration::Color::Magenta, 5, Decoration::Star | Decoration::Color::Green, 4, Decoration::Eraser | Decoration::Magenta, 1);
	generator->resetConfig();
	//Eraser + Stars + Shapes
	generator->setFlagOnce(Generate::Config::FixBackground);
	generator->generate(0x09DB3, Decoration::Star | Decoration::Color::Magenta, 5, Decoration::Poly | Decoration::Color::Orange, 2, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x09DB4, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Poly | Decoration::Color::Orange, 2, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x0A3CB, Decoration::Star | Decoration::Color::Magenta, 3, Decoration::Poly | Decoration::Color::Orange, 3, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x0A3CC, Decoration::Star | Decoration::Color::Magenta, 5, Decoration::Poly | Decoration::Color::Orange, 3, Decoration::Eraser | Decoration::White, 1);
	generator->generate(0x0A3D0, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Poly | Decoration::Color::Orange, 3, Decoration::Eraser | Decoration::White, 1);
}

void PuzzleList::GenerateBunkerN()
{
	//I would randomize this, if I could get the panels to actually render the symbols.
	//Unfortunately, the path is rendered to a 3D model that doesn't have any geometry between the grid lines.
	//Somehow, I would either have to change the model, or make the path render to the background texture instead.
}

void PuzzleList::GenerateSwampN()
{
	generator->setLoadingData(L"Swamp", 50);
	generator->resetConfig();
	//First row
	generator->setFlag(Generate::Config::SplitShapes);
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
	generator->setFlag(Generate::Config::RequireCombineShapes);
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
	specialCase->setTargetAndDeactivate(0x181A9, 0x00609);
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
	generator->setFlagOnce(Generate::Config::DisableReset);
	generator->setFlagOnce(Generate::Config::LongPath);
	generator->generate(0x17C0D, Decoration::Poly, 3);
	generator->place_gaps(6);
	generator->write(0x17C0E);
	//Disconnected Shapes
	generator->resetConfig();
	generator->setFlag(Generate::Config::DisconnectShapes);
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
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
	generator->setFlagOnce(Generate::Config::DisconnectShapes);
	generator->generate(0x00E3A, Decoration::Poly | Decoration::Can_Rotate, 3);
	//Optional Tetris
	generator->resetConfig();
	generator->generate(0x009A6, Decoration::Poly, 6);
	//Negative Shapes 1
	generator->resetConfig();
	generator->setFlag(Generate::Config::DisableCancelShapes);
	generator->setGridSize(3, 3);
	generator->pathWidth = 0.7f;
	generator->generate(0x009AB, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 1);
	generator->setGridSize(4, 4);
	generator->pathWidth = 0.65f;
	generator->generate(0x009AD, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 1);
	generator->generate(0x009AE, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 1);
	generator->generate(0x009AF, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 1);
	generator->setFlagOnce(Generate::Config::DisconnectShapes);
	generator->generate(0x00006, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 1);
	//Negative Shapes 2
	generator->generate(0x00002, Decoration::Poly, 2, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x00004, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x00005, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 2);
	generator->generate(0x013E6, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 1);
	generator->setFlagOnce(Generate::Config::DisconnectShapes);
	generator->generate(0x00596, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 1);
	//Negative Shapes 3
	generator->resetConfig();
	generator->setFlag(Generate::Config::RequireCancelShapes);
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
	generator->setLoadingData(L"Treehouse", 57);
	generator->resetConfig();
	generator->setFlag(Generate::Config::WriteColors); //Have to do this to get a proper looking orange and green
	generator->generate(0x02886, Decoration::Star | Decoration::Color::Orange, 2, Decoration::Gap, 5);
	generator->setFlag(Generate::Config::TreehouseLayout);
	//Yellow Bridge
	generator->setGridSize(3, 3);
	generator->generate(0x17D72, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Gap, 5);
	generator->setGridSize(4, 4);
	generator->generate(0x17D8F, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Gap, 10);
	generator->generate(0x17D74, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Gap, 10);
	generator->setGridSize(3, 3);
	generator->generate(0x17DAC, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Gap, 2);
	generator->setGridSize(4, 4);
	generator->generate(0x17D9E, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Gap, 5);
	generator->generate(0x17DB9, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Gap, 5);
	generator->generate(0x17D9C, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Gap, 5);
	generator->generate(0x17DC2, Decoration::Star | Decoration::Color::Orange, 8, Decoration::Gap, 2);
	generator->generate(0x17DC4, Decoration::Star | Decoration::Color::Orange, 8, Decoration::Gap, 2);
	generator->removeFlagOnce(Generate::Config::TreehouseLayout);
	generator->setGridSize(5, 5);
	generator->generate(0x0A182, Decoration::Star | Decoration::Color::Orange, 10, Decoration::Gap, 3);
	//Pink Bridge 1
	generator->setFlag(Generate::Config::FullGaps);
	generator->setGridSize(3, 3);
	generator->generate(0x17DC8, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Dot_Intersection, 4, Decoration::Gap, 2);
	generator->setGridSize(4, 4);
	generator->generate(0x17DC7, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Dot_Intersection, 6, Decoration::Gap, 3);
	generator->generate(0x17CE4, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Dot_Intersection, 6, Decoration::Gap, 3);
	generator->setGridSize(5, 5);
	generator->generate(0x17D2D, Decoration::Star | Decoration::Color::Magenta, 8, Decoration::Dot_Intersection, 9, Decoration::Gap, 5);
	generator->generate(0x17D6C, Decoration::Star | Decoration::Color::Magenta, 8, Decoration::Dot_Intersection, 9, Decoration::Gap, 5);
	generator->removeFlag(Generate::Config::FullGaps);
	//Pink Bridge 2
	generator->setGridSize(3, 3);
	generator->generate(0x17D9B, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2);
	generator->setGridSize(4, 4);
	generator->generate(0x17D99, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 4);
	generator->generate(0x17DAA, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3);
	generator->generate(0x17D97, Decoration::Star | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::Magenta, 4);
	generator->generate(0x17BDF, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Stone | Decoration::Color::Magenta, 3, Decoration::Stone | Decoration::Color::Green, 3);
	generator->setGridSize(5, 4);
	generator->setFlagOnce(Generate::Config::DisableReset);
	generator->generate(0x17D91, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Stone | Decoration::Color::Orange, 4, Decoration::Stone | Decoration::Color::Green, 4);
	generator->place_gaps(4);
	generator->write(0x17DC6);
	//Orange Bridge 1		
	generator->setGridSize(3, 3);
	generator->generate(0x17DB3, Decoration::Star | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2);
	generator->generate(0x17DB5, Decoration::Star | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2);
	generator->generate(0x17DB6, Decoration::Star | Decoration::Color::Black, 1, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2);
	generator->generate(0x17DC0, Decoration::Star | Decoration::Color::Black, 1, Decoration::Star | Decoration::Color::White, 1, Decoration::Stone | Decoration::Color::Black, 1, Decoration::Stone | Decoration::Color::White, 2);
	generator->setGridSize(4, 4);
	generator->removeFlag(Generate::Config::WriteColors);
	generator->setFlag(Generate::Config::AlternateColors); //Black -> Green, White -> Pink, Purple -> White
	generator->generate(0x17DD7, Decoration::Star | Decoration::Color::White, 1, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Purple, 2);
	generator->generate(0x17DD9, Decoration::Star | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Purple, 2);
	generator->generate(0x17DB8, Decoration::Star | Decoration::Color::White, 3, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Purple, 2);
	generator->generate(0x17DDC, Decoration::Star | Decoration::Color::White, 1, Decoration::Star | Decoration::Color::Black, 1, Decoration::Stone | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 2);
	generator->setGridSize(5, 5);
	generator->setObstructions({ { 1, 2 },{ 1, 4 },{ 9, 2 },{ 9, 4 },{ 2, 1 },{ 4, 1 },{ 6, 1 },{ 8, 1 } });
	generator->generate(0x17DD1, Decoration::Star | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 4 );
	generator->setGridSize(4, 4);
	generator->generate(0x17DDE, Decoration::Star | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::Black, 5, Decoration::Stone | Decoration::Color::White, 5);
	generator->setGridSize(3, 3);
	generator->generate(0x17DE3, Decoration::Star | Decoration::Color::Black, 2, Decoration::Star | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2);
	generator->removeFlag(Generate::Config::AlternateColors);
	generator->setGridSize(4, 4);
	generator->generate(0x17DEC, Decoration::Star | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 4);
	generator->generate(0x17DAE, Decoration::Star | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 4);
	generator->generate(0x17DB0, Decoration::Star | Decoration::Color::Black, 2, Decoration::Star | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 4);
	generator->generate(0x17DDB, Decoration::Star | Decoration::Color::Black, 3, Decoration::Star | Decoration::Color::White, 3, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3);
	//Orange Bridge 2
	generator->setFlag(Generate::Config::WriteColors);
	generator->setGridSize(3, 3);
	generator->generate(0x17D88, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Gap, 2);
	generator->setGridSize(4, 4);
	generator->generate(0x17DB4, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Gap, 8);
	generator->generate(0x17D8C, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Gap, 6);
	generator->setObstructions({ { 1, 2 },{ 1, 4 },{ 7, 2 },{ 7, 4 },{ 2, 1 },{ 4, 1 },{ 6, 1 } });
	generator->generate(0x17CE3, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Star | Decoration::Color::Magenta, 4);
	generator->generate(0x17DCD, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Gap, 3);
	generator->generate(0x17DB2, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Gap, 3);
	generator->generate(0x17DCC, Decoration::Star | Decoration::Color::Orange, 8, Decoration::Star | Decoration::Color::Magenta, 4);
	generator->generate(0x17DCA, Decoration::Star | Decoration::Color::Magenta, 8, Decoration::Star | Decoration::Color::Orange, 6);
	generator->generate(0x17D8E, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Star | Decoration::Color::Magenta, 2, Decoration::Star | Decoration::Color::Green, 2, Decoration::Gap, 8);
	generator->setObstructions({ { 1, 2 },{ 1, 4 },{ 7, 2 },{ 7, 4 },{ 2, 1 },{ 4, 1 },{ 6, 1 } });
	generator->generate(0x17DB7, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Star | Decoration::Color::Green, 4);
	generator->setGridSize(4, 3);
	generator->generate(0x17DB1, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Star | Decoration::Color::Green, 2);
	generator->setGridSize(5, 5);
	generator->generate(0x17DA2, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Star | Decoration::Color::Magenta, 6, Decoration::Star | Decoration::Color::Green, 6, Decoration::Star | Decoration::Color::Black, 6);
	//Green Bridge
	generator->removeFlag(Generate::Config::WriteColors);
	generator->setGridSize(4, 4);
	generator->generate(0x17E3C, Decoration::Poly | Decoration::Color::White, 2, Decoration::Star | Decoration::Color::Black, 4);
	generator->generate(0x17E4D, Decoration::Poly | Decoration::Color::White, 2, Decoration::Star | Decoration::Color::Black, 4);
	generator->generate(0x17E4F, Decoration::Poly | Decoration::Color::White, 1, Decoration::Poly | Decoration::Can_Rotate | Decoration::Color::White, 1, Decoration::Star | Decoration::Color::Black, 6);
	generator->setGridSize(5, 5);
	generator->setObstructions({ { { 1, 2 },{ 1, 4 },{ 9, 2 },{ 9, 4 },{ 2, 1 },{ 4, 1 },{ 6, 1 },{ 8, 1 } },
		{ { 1, 2 },{ 1, 4 },{ 1, 6 },{ 0, 7 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 7 },{ 4, 1 },{ 6, 1 },{ 8, 1 } },
		{ { 1, 2 },{ 1, 4 },{ 1, 6 },{ 0, 7 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 10, 7 },{ 2, 1 },{ 4, 1 },{ 6, 1 } } });
	generator->generate(0x17E52, Decoration::Poly | Decoration::Can_Rotate | Decoration::Color::White, 2, Decoration::Star | Decoration::Color::Black, 6);
	generator->generate(0x17E5B, Decoration::Poly | Decoration::Color::White, 2, Decoration::Poly | Decoration::Color::Black, 1, Decoration::Star | Decoration::Color::Black, 5);
	generator->setGridSize(4, 4);
	generator->generate(0x17E5F, Decoration::Poly | Decoration::Color::White, 2, Decoration::Poly | Decoration::Negative | Decoration::Color::Black, 1, Decoration::Star | Decoration::Color::Black, 5);
	generator->setGridSize(5, 5);
	generator->generate(0x17E61, Decoration::Poly | Decoration::Can_Rotate | Decoration::Color::White, 2, Decoration::Poly | Decoration::Color::White, 1, Decoration::Star | Decoration::Color::Black, 4, Decoration::Star | Decoration::Color::Purple, 4);
}

void PuzzleList::GenerateTownN()
{
	generator->setLoadingData(L"Town", 20);
	generator->resetConfig();
	//Full Dots + Shapes
	generator->generate(0x2899C, Decoration::Dot_Intersection, 25, Decoration::Poly | Decoration::Can_Rotate, 1);
	generator->generate(0x28A33, Decoration::Dot_Intersection, 25, Decoration::Poly, 2);
	generator->generate(0x28ABF, Decoration::Dot_Intersection, 25, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Poly, 1);
	generator->generate(0x28AC0, Decoration::Dot_Intersection, 25, Decoration::Poly | Decoration::Can_Rotate, 2);
	generator->generate(0x28AC1, Decoration::Dot_Intersection, 25, Decoration::Poly | Decoration::Can_Rotate, 3);
	generator->generate(0x28AD9, Decoration::Dot_Intersection, 25, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Eraser | Decoration::Color::White, 1);
	//Blue Symmetry
	generator->setSymmetry(Panel::Symmetry::Rotational);
	generator->generate(0x28AC7, Decoration::Stone | Decoration::Color::Black, 6, Decoration::Stone | Decoration::Color::White, 6);
	generator->generate(0x28AC8, Decoration::Stone | Decoration::Color::Black, 6, Decoration::Stone | Decoration::Color::White, 6);
	generator->generate(0x28ACA, Decoration::Stone | Decoration::Color::Black, 5, Decoration::Stone | Decoration::Color::White, 5, Decoration::Dot, 3);
	generator->generate(0x28ACB, Decoration::Stone | Decoration::Color::Black, 5, Decoration::Stone | Decoration::Color::White, 5, Decoration::Dot, 3);
	generator->generate(0x28ACC, Decoration::Stone | Decoration::Color::Black, 5, Decoration::Stone | Decoration::Color::White, 5, Decoration::Dot, 3);
	generator->setSymmetry(Panel::Symmetry::None);
	//Glass Door
	generator->setFlagOnce(Generate::Config::SmallShapes);
	generator->generate(0x28998, Decoration::Poly | Decoration::Can_Rotate, 4, Decoration::Star | Decoration::Color::White, 6);
	//Church Star Door
	specialCase->generateColorFilterPuzzle(0x28A0D, { std::make_pair<int,int>(Decoration::Star | 1, 6),
		std::make_pair<int,int>(Decoration::Star | 2, 6), std::make_pair<int,int>(Decoration::Star | 3, 4) }, { 1, 1, 0, 0 });
	specialCase->setTarget(0x28A0D, 0x03B6F); specialCase->clearTarget(0x28A69); //Set star door to open tower instead of perspective puzzle (to prevent cheesing it)
	//Soundproof Room
	std::vector<int> allPitches = { DOT_SMALL, DOT_SMALL, DOT_MEDIUM, DOT_MEDIUM, DOT_LARGE, DOT_LARGE };
	std::vector<int> pitches;
	for (int i = 0; i < 4; i++) pitches.push_back(pop_random(allPitches));
	specialCase->generateSoundDotPuzzle(0x034E3, pitches, false);
	generator->resetConfig();
	//3-color Room
	specialCase->generateRGBStonePuzzleN(0x03C0C);
	specialCase->generateRGBStarPuzzleN(0x03C08);
	//Orange Crate
	generator->generate(0x0A0C8, Decoration::Poly | Decoration::Color::Orange, 3, Decoration::Stone | Decoration::Color::White, 2, Decoration::Stone | Decoration::Color::Black, 2);
	//Windmill Puzzles
	//The Witness has a weird issue with these particular puzzles where the edge at (3, 4) gets bypassed by the region calculator if you don't draw over it.
	//Because of this, polyominoes can't be used with these puzzles unless Config::PreserveStructure flag is turned off.
	generator->setFlagOnce(Generate::Config::PreserveStructure);
	generator->generate(0x17F89, Decoration::Stone | Decoration::Color::Black, 5, Decoration::Stone | Decoration::Color::White, 5, Decoration::Gap, 7);
	generator->setFlag(Generate::Config::FixBackground);
	generator->setFlagOnce(Generate::Config::SplitErasers);
	generator->generate(0x0A168, Decoration::Stone | Decoration::Color::White, 8, Decoration::Stone | Decoration::Color::Black, 8, Decoration::Eraser | Decoration::Color::White, 2);
	generator->setFlagOnce(Generate::Config::WriteColors);
	generator->generate(0x33AB2, Decoration::Poly | Decoration::Color::Orange, 2, Decoration::Stone | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Gap, 4);
}

void PuzzleList::GenerateVaultsN()
{
	generator->setLoadingData(L"Vaults", 5);
	generator->resetConfig();
	//Tutorial Vault
	generator->generate(0x033D4, Decoration::Stone | Decoration::Color::White, 10, Decoration::Stone | Decoration::Color::Black, 10, Decoration::Dot, 10, Decoration::Start, 4);
	//Tetris Vault
	generator->setFlagOnce(Generate::Config::RequireCancelShapes);
	generator->generate(0x0CC7B, Decoration::Dot_Intersection, 49, Decoration::Poly, 1, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Poly | Decoration::Negative, 3);
	//Symmetry Vault
	generator->setSymmetry(Panel::Symmetry::Rotational);
	generator->generate(0x002A6, Decoration::Stone | Decoration::Color::White, 4, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Dot, 2,
		Decoration::Dot | Decoration::Color::Blue, 2, Decoration::Dot | Decoration::Color::Yellow, 2);
	generator->resetConfig();
	//Shipwreck Vault
	generator->setGridSize(7, 7);
	specialCase->generateSoundDotReflectionPuzzle(0x00AFB, { DOT_MEDIUM, DOT_LARGE, DOT_MEDIUM, DOT_SMALL }, { DOT_LARGE, DOT_SMALL, DOT_MEDIUM }, 3, false);
	generator->resetConfig();
	//Jungle Vault
	specialCase->generateJungleVault(0x15ADD);
}

void PuzzleList::GenerateTrianglePanelsN()
{
	generator->setLoadingData(L"Triangles", 12);
	generator->resetConfig();
	generator->setGridSize(4, 4);
	generator->generate(0x17CFB, Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Gap, 8);
	generator->generate(0x3C12B, Decoration::Triangle | Decoration::Color::Orange, 6);
	generator->generate(0x17CE7, Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Gap, 8);
	generator->generate(0x17CF0, Decoration::Triangle | Decoration::Color::Orange, 6);
	generator->generate(0x17FA9, Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Gap, 8);
	generator->generate(0x17FA0, Decoration::Triangle | Decoration::Color::Orange, 6);
	generator->generate(0x17D27, Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Gap, 8);
	generator->generate(0x17D28, Decoration::Triangle | Decoration::Color::Orange, 6);
	generator->generate(0x17D01, Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Gap, 8);
	generator->generate(0x17C71, Decoration::Triangle | Decoration::Color::Orange, 6);
	generator->generate(0x17CF7, Decoration::Triangle | Decoration::Color::Orange, 4, Decoration::Gap, 8);
	generator->generate(0x17C42, Decoration::Triangle | Decoration::Color::Orange, 6);
}

void PuzzleList::GenerateMountainN()
{
	std::wstring text = L"Mountain Perspective (might take a while)";
	SetWindowText(_handle, text.c_str());
	specialCase->generateMountaintop(0x17C34); //TODO: Improve the speed on this
	
	generator->setLoadingData(L"Mountain", 31);
	generator->resetConfig();
	//Purple Bridge
	generator->setFlagOnce(Generate::Config::PreserveStructure);
	generator->hitPoints = { { 5, 6 },{ 7, 6 },{ 7, 4 } };
	generator->setObstructions({ { 4, 1 },{ 6, 1 },{ 8, 1 } });
	generator->setSymbol(Decoration::Gap_Row, 3, 4);
	generator->setFlagOnce(Generate::Config::DisableWrite);
	generator->generate(0x09E39, Decoration::Stone | Decoration::Color::Purple, 3, Decoration::Stone | Decoration::Color::White, 3,
		Decoration::Stone | Decoration::Color::Black, 3, Decoration::Eraser | Decoration::Color::White, 1);
	generator->setVal(Decoration::Dot_Intersection, 6, 6);
	generator->setFlagOnce(Generate::Config::DecorationsOnly);
	generator->write(0x09E39);
	
	//Orange Row
	generator->resetConfig();
	generator->generate(0x09E73, Decoration::Dot, 4, Decoration::Stone | Decoration::Color::Black, 2, Decoration::Stone | Decoration::Color::White, 2,
		Decoration::Star | Decoration::Color::Black, 2, Decoration::Star | Decoration::Color::White, 2);
	generator->generate(0x09E75, Decoration::Dot, 4, Decoration::Stone | Decoration::Color::Black, 1, Decoration::Stone | Decoration::Color::White, 3,
		Decoration::Star | Decoration::Color::Black, 3, Decoration::Star | Decoration::Color::White, 1);
	generator->generate(0x09E78, Decoration::Poly, 2, Decoration::Dot, 2);
	generator->generate(0x09E79, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Poly, 1, Decoration::Gap, 3);
	generator->generate(0x09E6C, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 4,
		Decoration::Star | Decoration::Color::Purple, 4, Decoration::Gap, 2);
	generator->generate(0x09E6F, Decoration::Poly, 2, Decoration::Star | Decoration::Color::Magenta, 4);
	generator->generate(0x09E6B, Decoration::Dot, 8, Decoration::Star | Decoration::Color::Magenta, 4);
	//Green Row
	generator->generate(0x09E7A, Decoration::Dot, 6, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3);
	generator->generate(0x09E71, Decoration::Dot, 6, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 2);
	generator->generate(0x09E72, Decoration::Dot, 4, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 2, Decoration::Poly, 1);
	generator->generate(0x09E69, Decoration::Dot, 6, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3);
	generator->generate(0x09E7B, Decoration::Dot, 10, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 2);
	//Purple Panels
	generator->setFlag(Generate::Config::RequireCombineShapes);
	generator->generate(0x09EAD, Decoration::Poly, 2, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3);
	generator->generate(0x09EAF, Decoration::Poly, 2, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3);
	generator->resetConfig();
	//Blue Row
	generator->setObstructions({ { 4, 3 },{ 5, 4 },{ 5, 6 },{ 5, 8 },{ 5, 10 },{ 6, 9 },{ 7, 10 } });
	generator->generate(0x33AF5, Decoration::Dot, 3, Decoration::Stone | Decoration::Color::Black, 5, Decoration::Stone | Decoration::Color::White, 5);
	if (rand() % 2 == 0) generator->setObstructions({ { 5, 4 },{ 5, 6 },{ 5, 8 },{ 5, 10 },{ 9, 4 },{ 9, 6 },{ 9, 8 },{ 9, 10 },{ 7, 0 },{ 7, 2 } });
	else generator->setObstructions({ { 3, 4 },{ 3, 6 },{ 3, 8 },{ 3, 10 },{ 7, 4 },{ 7, 6 },{ 7, 8 },{ 7, 10 },{ 5, 0 },{ 5, 2 },{ 9, 0 },{ 9, 2 } });
	generator->generate(0x33AF7, Decoration::Stone | Decoration::Color::Black, 7, Decoration::Stone | Decoration::Color::White, 5, Decoration::Star | Decoration::Color::Orange, 4);
	generator->setObstructions({ { 0, 1 },{ 0, 3 },{ 0, 5 },{ 0, 7 },{ 9, 4 },{ 1, 4 },{ 1, 6 },{ 1, 8 },{ 2, 7 },{ 2, 9 },{ 3, 8 },{ 3, 10 },{ 4, 9 },{ 5, 8 },{ 5, 10 },
		{ 6, 7 },{ 6, 9 },{ 7, 6 },{ 7, 8 },{ 7, 10 },{ 8, 5 },{ 8, 7 },{ 8, 9 },{ 9, 2 },{ 9, 4 },{ 9, 6 },{ 9, 8 },{ 10, 1 },{ 10, 3 },{ 10, 5 } });
	generator->generate(0x09F6E, Decoration::Dot, 6);

	//Rainbow Row
	generator->resetConfig();
	generator->setFlag(Generate::Config::WriteColors);
	generator->generate(0x09FD3, Decoration::Stone | Decoration::Color::Magenta, 2, Decoration::Stone | Decoration::Color::Green, 2,
		Decoration::Star | Decoration::Color::Magenta, 2, Decoration::Star | Decoration::Color::Green, 2);
	generator->generate(0x09FD4, Decoration::Stone | Decoration::Color::Magenta, 2, Decoration::Stone | Decoration::Color::Green, 2,
		Decoration::Star | Decoration::Color::Magenta, 2, Decoration::Star | Decoration::Color::Green, 3);
	generator->generate(0x09FD6, Decoration::Stone | Decoration::Color::Cyan, 3, Decoration::Stone | Decoration::Color::Yellow, 2,
		Decoration::Star | Decoration::Color::Cyan, 3, Decoration::Star | Decoration::Color::Yellow, 4);
	generator->generate(0x09FD7, { { Decoration::Stone | Decoration::Color::Cyan, 2 },{ Decoration::Stone | Decoration::Color::Magenta, 2 },
		{ Decoration::Star | Decoration::Color::Cyan, 3 },{ Decoration::Star | Decoration::Color::Magenta, 3 },
		{ Decoration::Poly | Decoration::Color::Cyan, 1 },{ Decoration::Poly | Decoration::Color::Magenta, 1 } });
	generator->setFlagOnce(Generate::Config::Write2Color);
	generator->setSymbol(Decoration::Start, 10, 0);
	generator->setSymbol(Decoration::Start, 0, 10);
	generator->setSymbol(Decoration::Exit, 0, 0);
	generator->setSymbol(Decoration::Exit, 10, 10);
	generator->setSymmetry(Panel::Symmetry::Rotational);
	generator->generate(0x09FD8, Decoration::Dot_Intersection | Decoration::Color::Blue, 4, Decoration::Dot_Intersection | Decoration::Color::Orange, 4);
	generator->resetConfig();

	specialCase->generateMultiPuzzle({ 0x09FCC, 0x09FCE, 0x09FCF, 0x09FD0, 0x09FD1, 0x09FD2 }, {
		{ { Decoration::Dot_Intersection, 3 } },
		{ { Decoration::Stone | Decoration::Color::Black, 2 },{ Decoration::Stone | Decoration::Color::White, 2 } },
		{ { Decoration::Star | Decoration::Color::Magenta, 2 },{ Decoration::Star | Decoration::Color::Green, 2 } },
		{ { Decoration::Poly | Decoration::Can_Rotate, 1 } },
		{ { Decoration::Stone | Decoration::Color::Cyan, 1 },{ Decoration::Stone | Decoration::Color::Yellow, 1 },{ Decoration::Star | Decoration::Color::Cyan, 1 },{ Decoration::Star | Decoration::Color::Yellow, 1 } },
		{ { Decoration::Poly, 3 },{ Decoration::Eraser | Decoration::Color::White , 1 } } });

	specialCase->generate2Bridge(0x09E86, 0x09ED8);

	specialCase->generateMountainFloor({ 0x09EFF, 0x09F01, 0x09FC1, 0x09F8E }, 0x09FDA);

	//Pillar Puzzles
	generator->setGridSize(6, 3);
	generator->generate(0x0383D, Decoration::Dot, 15, Decoration::Gap, 6);
	generator->resetConfig();
	generator->setFlag(Generate::Config::WriteColors);
	generator->generate(0x0383A, Decoration::Star | Decoration::Color::Orange, 6, Decoration::Star | Decoration::Color::Magenta, 4);
	generator->generate(0x0383F, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 4);
	generator->generate(0x09E56, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Dot, 8);
	generator->removeFlag(Generate::Config::WriteColors);
	generator->setFlagOnce(Generate::Config::RequireCombineShapes);
	generator->generate(0x03859, Decoration::Poly, 2);
	generator->setSymbol(Decoration::Start, 4, 8);
	generator->generate(0x09E5A, Decoration::Dot_Intersection, 30, Decoration::Gap, 10);
	generator->setFlagOnce(Generate::Config::WriteColors);
	generator->setSymmetry(Panel::Symmetry::PillarRotational);
	generator->generate(0x339BB, Decoration::Star | Decoration::Color::Orange, 4, Decoration::Stone | Decoration::Color::Black, 4,
		Decoration::Stone | Decoration::Color::White, 4);
	generator->setSymmetry(Panel::Symmetry::PillarParallel);
	generator->setFlagOnce(Generate::Config::DisableDotIntersection);
	generator->generate(0x33961, Decoration::Dot, 8);
}

void PuzzleList::GenerateCavesN()
{
	generator->setLoadingData(L"Caves", 51);
	generator->resetConfig();
	generator->generate(0x17FA2, Decoration::Triangle | Decoration::Color::Orange, 12);
	generator->generate(0x00FF8, Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 3,
		Decoration::Triangle | Decoration::Color::Orange, 4);

	//Tetris Puzzles
	generator->setFlag(Generate::Config::RequireCombineShapes);
	generator->generate(0x01A0D, Decoration::Poly, 4);
	generator->setGridSize(5, 5);
	generator->generate(0x018A0, Decoration::Poly, 4);
	generator->resetConfig();
	generator->setFlag(Generate::Config::DisconnectShapes);
	generator->generate(0x009A4, Decoration::Poly, 4);
	generator->generate(0x00A72, Decoration::Poly, 3, Decoration::Poly | Decoration::Negative, 1);
	generator->resetConfig();

	generator->setFlag(Generate::Config::BigShapes);
	generator->generate(0x00190, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Triangle | Decoration::Color::Orange, 4);
	generator->generate(0x00558, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Triangle | Decoration::Color::Orange, 4);
	generator->generate(0x00567, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Triangle | Decoration::Color::Orange, 4);
	generator->generate(0x006FE, Decoration::Poly | Decoration::Can_Rotate, 1, Decoration::Triangle | Decoration::Color::Orange, 4);
	generator->resetConfig();

	generator->generate(0x008B8, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Triangle | Decoration::Color::Orange, 3);
	generator->generate(0x00973, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Triangle | Decoration::Color::Orange, 3);
	generator->generate(0x0097B, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Triangle | Decoration::Color::Orange, 3);
	generator->generate(0x0097D, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Triangle | Decoration::Color::Orange, 3);
	generator->generate(0x0097E, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Triangle | Decoration::Color::Orange, 3);

	generator->generate(0x00994, Decoration::Poly, 3, Decoration::Triangle | Decoration::Color::Orange, 4);
	generator->generate(0x334D5, Decoration::Poly, 3, Decoration::Triangle | Decoration::Color::Orange, 4);
	generator->generate(0x00995, Decoration::Poly, 3, Decoration::Triangle | Decoration::Color::Orange, 4);
	generator->generate(0x00996, Decoration::Poly, 3, Decoration::Triangle | Decoration::Color::Orange, 5);
	generator->generate(0x00998, Decoration::Poly, 3, Decoration::Triangle | Decoration::Color::Orange, 5);

	//First alcove
	generator->setFlagOnce(Generate::Config::DisconnectShapes);
	generator->generate(0x32962, Decoration::Poly | Decoration::Can_Rotate, 3, Decoration::Gap, 3);
	generator->setFlagOnce(Generate::Config::WriteColors);
	generator->generate(0x32966, Decoration::Star | Decoration::Color::Black, 4, Decoration::Star | Decoration::Color::White, 4,
		Decoration::Star | Decoration::Color::Orange, 4, Decoration::Stone | Decoration::Color::Black, 1, Decoration::Stone | Decoration::Color::White, 1);
	generator->setFlagOnce(Generate::Config::WriteColors);
	generator->generate(0x01A31, Decoration::Stone | Decoration::Color::Cyan, 16, Decoration::Stone | Decoration::Color::Yellow, 14);
	generator->setGridSize(4, 4);
	generator->generate(0x00B71, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3,
		Decoration::Star | Decoration::Color::Black, 3, Decoration::Star | Decoration::Color::White, 3, Decoration::Eraser | Decoration::Color::White, 1);
	generator->resetConfig();

	//Perspective
	generator->setFlag(Generate::Config::DecorationsOnly);
	std::vector<std::vector<Point>> obstructions = { { { 5, 0 },{ 5, 2 },{ 5, 4 } },{ { 5, 6 },{ 5, 8 },{ 5, 10 } },{ { 0, 5 },{ 2, 5 },{ 4, 5 } },{ { 6, 5 },{ 8, 5 },{ 10, 5 } } };
	generator->setObstructions(obstructions);
	generator->blockPos = { { 5, 5 } };
	generator->generate(0x288EA, Decoration::Poly, 3);
	generator->setObstructions(obstructions);
	generator->blockPos = { { 5, 5 } };
	generator->generate(0x288FC, Decoration::Poly, 1, Decoration::Poly | Decoration::Can_Rotate, 1,
		Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3);
	generator->setObstructions(obstructions);
	generator->blockPos = { { 5, 5 } };
	generator->generate(0x289E7, Decoration::Stone | Decoration::Color::Black, 3, Decoration::Stone | Decoration::Color::White, 3,
		Decoration::Star | Decoration::Color::Black, 4, Decoration::Star | Decoration::Color::White, 4);
	generator->setObstructions(obstructions);
	generator->blockPos = { { 5, 5 } };
	generator->setFlagOnce(Generate::Config::RequireCombineShapes);
	generator->generate(0x288AA, Decoration::Poly, 2, Decoration::Star | Decoration::Color::Black, 6, Decoration::Star | Decoration::Color::White, 4);
	generator->resetConfig();

	//Full Dots
	generator->generate(0x0A16B, Decoration::Dot_Intersection, 25, Decoration::Stone | Decoration::Color::Black, 1, Decoration::Stone | Decoration::Color::White, 1,
		Decoration::Stone | Decoration::Color::Cyan, 1, Decoration::Stone | Decoration::Color::Magenta, 1);
	generator->generate(0x0A2CE, Decoration::Dot_Intersection, 25, Decoration::Star | Decoration::Color::Black, 6, Decoration::Star | Decoration::Color::White, 2);
	generator->generate(0x0A2D7, Decoration::Dot_Intersection, 25, Decoration::Triangle | Decoration::Color::Black, 4);
	generator->generate(0x0A2DD, Decoration::Dot_Intersection, 25, Decoration::Poly | Decoration::Color::Black, 3);
	generator->generate(0x0A2EA, Decoration::Dot_Intersection, 25, Decoration::Poly | Decoration::Can_Rotate | Decoration::Color::Black, 2);
	generator->setSymbol(Decoration::Start, 0, 0); generator->setSymbol(Decoration::Start, 6, 6); generator->setSymbol(Decoration::Start, 0, 6);
	generator->generate(0x17FB9, Decoration::Dot_Intersection, 16, Decoration::Poly | Decoration::Color::Black, 2,
		Decoration::Poly | Decoration::Negative | Decoration::Color::Black, 1);
	generator->resetConfig();

	//Invisible Dots
	generator->generate(0x0008F, Decoration::Dot_Intersection, 2);
	generator->generate(0x0006B, Decoration::Dot, 3);
	generator->generate(0x0008B, Decoration::Dot, 6);
	generator->generate(0x0008C, Decoration::Dot, 6);
	generator->generate(0x0008A, Decoration::Dot, 9);
	generator->generate(0x00089, Decoration::Dot, 9);
	generator->generate(0x0006A, Decoration::Dot, 9);
	generator->generate(0x0006C, Decoration::Dot, 12);
	generator->generate(0x00027, Decoration::Dot, 3);
	generator->generate(0x00028, Decoration::Dot, 7);
	generator->generate(0x00029, Decoration::Dot, 7);

	//Triangle Exit
	generator->generate(0x17CF2, Decoration::Triangle1 | Decoration::Color::Orange, 5);
	//Stars Exit
	generator->generate(0x021D7, Decoration::Triangle | Decoration::Color::Cyan, 2, Decoration::Triangle | Decoration::Color::Yellow, 2,
		Decoration::Star | Decoration::Color::Cyan, 2, Decoration::Star | Decoration::Color::Yellow, 2);

	//Challenge Pillar
	generator->generate(0x09DD5, Decoration::Triangle | Decoration::Color::Orange, 12);
	//Challenge Entrance
	generator->generate(0x0A16E, Decoration::Poly | Decoration::Color::Green, 4, Decoration::Star | Decoration::Color::Green, 3);
	//Theater Exit
	generator->generate(0x039B4, Decoration::Triangle2 | Decoration::Color::Orange, 12);
	//Town Exit
	generator->generate(0x09E85, Decoration::Triangle | Decoration::Color::Orange, 16, Decoration::Start, 4);
}

void PuzzleList::GenerateOrchardN()
{
	specialCase->generateApplePuzzle(0x032FF, true, false);
}

void PuzzleList::GenerateKeepN()
{
	generator->setLoadingData(L"Keep", 5);
	generator->resetConfig();

	//TODO: Randomize the last hedge maze?

	//Pressure Plate Puzzles

	generator->resetConfig();
	generator->setSymbol(Decoration::Gap_Column, 8, 3);
	generator->setSymbol(Decoration::Gap_Column, 4, 5);
	generator->setSymbol(Decoration::Gap_Row, 3, 0);
	generator->setSymbol(Decoration::Gap_Row, 3, 2);
	generator->setSymbol(Decoration::Gap_Row, 5, 6);
	generator->setFlagOnce(Generate::Config::DisableWrite);
	generator->setFlagOnce(Generate::Config::LongPath);
	generator->generate(0x033EA);
	std::set<Point> path1 = generator->_path;
	std::vector<std::vector<Point>> sets = { { {7, 8}, {8, 7} }, { { 6, 5 }, {7, 4} }, { {7, 0}, {7, 2}, {6, 1}, {8, 1}, {5, 2}, {5, 4} },
		{ {2, 7}, {4, 7}, {3, 8}, {3, 6}, {1, 6}, {1, 4} }, { {0, 1}, {1, 0}, {2, 1}, {1, 2} } };
	for (std::vector<Point> set : sets) {
		Point p = pick_random(set);
		while (!path1.count(p)) p = pick_random(set);
		generator->set(p, p.first % 2 == 0 ? Decoration::Dot_Column : Decoration::Dot_Row);
	}
	generator->write(0x033EA);

	generator->resetConfig();
	generator->setSymbol(Decoration::Gap_Row, 3, 2);
	generator->setSymbol(Decoration::Gap_Column, 8, 5);
	generator->setFlagOnce(Generate::Config::DisableWrite);
	generator->generate(0x01BE9, Decoration::Star | Decoration::Color::Magenta, 4, Decoration::Star | Decoration::Color::Cyan, 4,
		Decoration::Stone | Decoration::Color::Black, 4, Decoration::Stone | Decoration::Color::White, 4);
	generator->set(3, 2, 0);
	generator->set(8, 5, 0);
	std::set<Point> path2 = generator->_path;
	generator->write(0x01BE9);

	generator->resetConfig();
	std::vector<std::vector<Point>> validHitPoints = {
		{ { 5, 8 },{ 3, 4 },{ 7, 2 },{ 3, 2 } },{ { 8, 7 },{ 7, 2 },{ 6, 7 },{ 5, 8 } },{ { 5, 8 },{ 4, 5 },{ 7, 2 },{ 2, 5 } },
		{ { 5, 8 },{ 3, 6 },{ 7, 2 },{ 3, 4 } },{ { 5, 8 },{ 1, 6 },{ 7, 2 },{ 1, 4 } },{ { 5, 8 },{ 4, 3 },{ 7, 2 },{ 2, 3 } },
		{ { 5, 8 },{ 3, 4 },{ 7, 2 },{ 3, 2 } },{ { 5, 8 },{ 1, 4 },{ 7, 2 },{ 1, 2 } },{ { 5, 8 },{ 3, 2 },{ 7, 2 },{ 3, 0 } },
		{ { 5, 8 },{ 1, 2 },{ 7, 2 },{ 1, 0 } } };
	generator->hitPoints = validHitPoints[rand() % validHitPoints.size()];
	generator->setFlagOnce(Generate::Config::SplitShapes);
	generator->setFlagOnce(Generate::Config::DisableWrite);
	generator->generate(0x01CD3, Decoration::Poly, 2, Decoration::Stone | Decoration::Color::Black, 1, Decoration::Stone | Decoration::Color::White, 1,
		Decoration::Stone | Decoration::Color::Cyan, 1, Decoration::Stone | Decoration::Color::Magenta, 1);
	std::set<Point> path3 = generator->_path;
	generator->write(0x01CD3);

	generator->resetConfig();
	generator->setSymmetry(Panel::Symmetry::Rotational);
	generator->setFlagOnce(Generate::Config::DisableWrite);
	generator->generate(0x01D3F, Decoration::Poly | Decoration::Can_Rotate, 2, Decoration::Poly, 1);
	std::swap(generator->_panel->_endpoints[0], generator->_panel->_endpoints[1]); //Need to have endpoints in right order to associate with pressure plates correctly
	std::set<Point> path4 = (generator->_path1.count(Point(0, 8)) ? generator->_path2 : generator->_path1);
	if (generator->_path.count({ 7, 0 })) generator->set(7, 0, Decoration::Dot_Row);
	else generator->set(8, 1, Decoration::Dot_Column);
	generator->write(0x01D3F);
	
	specialCase->generateKeepLaserPuzzle(0x03317, path1, path2, path3, path4,
		{ { Decoration::Dot, 4 },{ Decoration::Star | Decoration::Color::Magenta, 4 },{ Decoration::Star | Decoration::Color::Cyan, 4 },
		{ Decoration::Stone | Decoration::Color::Black, 5 },{ Decoration::Stone | Decoration::Color::White, 5 },{ Decoration::Stone | Decoration::Color::Cyan, 1 },
		{ Decoration::Stone | Decoration::Color::Magenta, 1 },{ Decoration::Poly, 3 },{ Decoration::Poly | Decoration::Can_Rotate, 2 } });

	specialCase->clearTarget(0x0360E); //Must solve pressure plate side
}

void PuzzleList::GenerateJungleN()
{
	//Can't random sound cues just yet. I found the file names in memory, but have yet to figure out where the actual sound files are.
	//specialCase->testFind({ 'b', 'i', 'r', 'd', '3' }); //0x52a230
	//specialCase->testFind({ 'b', 'i', 'r', 'd', '4' }); //0x52a2d8
	//So the only thing that can be randomized currently, are the actual solutions.

	generator->setLoadingData(L"Jungle", 4);
	generator->resetConfig();

	specialCase->generateSoundDotPuzzle(0x0026F, { DOT_MEDIUM, DOT_MEDIUM, DOT_SMALL, DOT_MEDIUM, DOT_LARGE }, false);
	if (rand() % 2) specialCase->generateSoundDotPuzzle(0x00C3F, { DOT_SMALL, DOT_MEDIUM, DOT_SMALL, DOT_LARGE }, true);
	else specialCase->generateSoundDotPuzzle(0x00C3F, { DOT_LARGE, DOT_MEDIUM, DOT_MEDIUM, DOT_SMALL, DOT_LARGE }, true);
	if (rand() % 2) specialCase->generateSoundDotPuzzle(0x00C41, { DOT_SMALL, DOT_SMALL, DOT_LARGE, DOT_MEDIUM, DOT_LARGE }, true);
	else specialCase->generateSoundDotPuzzle(0x00C41, { DOT_MEDIUM, DOT_MEDIUM, DOT_SMALL, DOT_MEDIUM, DOT_LARGE }, true);
	if (rand() % 2) specialCase->generateSoundDotPuzzle(0x014B2, { DOT_SMALL, DOT_LARGE, DOT_SMALL, DOT_LARGE, DOT_MEDIUM }, true);
	else specialCase->generateSoundDotPuzzle(0x014B2, { DOT_LARGE, DOT_MEDIUM, DOT_SMALL, DOT_LARGE, DOT_SMALL }, true);
}

//--------------------------HARD MODE-----------------------------

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
	/*
	generator->resetConfig();
	generator->setFlag(Generate::Config::WriteColors);
	generator->setGridSize(6, 6);
	generator->setSymbol(IntersectionFlags::OPEN, 11, 0);
	generator->setSymbol(IntersectionFlags::OPEN, 12, 1);
	generator->setSymbol(IntersectionFlags::NO_POINT, 12, 0);
	generator->setSymbol(Decoration::Empty, 11, 1);
	generator->setSymbol(Decoration::Exit, 12, 2);

	generator->generate(0x09FD8, Decoration::Stone, 34);
	*/
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
