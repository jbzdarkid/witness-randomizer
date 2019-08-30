#pragma once
#include "Generate.h"

class PuzzleList {

public: 

	void GenerateAllN();
	void GenerateAllH();

	PuzzleList() {
		generator = std::make_shared<Generate>();
	}

	PuzzleList(std::shared_ptr<Generate> generator) {
		this->generator = generator;
	}

	void setLoadingHandle(HWND handle) {
		_handle = handle;
		generator->setLoadingHandle(handle);
	}

	//--------------------------Normal difficulty---------------------------

	void GenerateTutorialN();
	void GenerateSymmetryN();
	void GenerateQuarryN();
	void GenerateBunkerN();
	void GenerateSwampN();
	void GenerateTreehouseN();
	void GenerateTownN();
	void GenerateVaultsN();
	void GenerateTrianglePanelsN();
	void GenerateMountainN();
	void GenerateCavesN();

	//Environmental areas - unless I can figure out how to mess with the game's assets, randomizing most of these puzzles isn't happening anytime soon
	void GenerateOrchardN();
	void GenerateDesertN(); //Just scramble the positions for now
	void GenerateShadowsN(); //Can't randomize
	void GenerateKeepN();
	void GenerateMonasteryN(); //Can't randomize
	void GenerateJungleN();

	//-------------------------Hard difficulty--------------------------

	void GenerateTutorialH();
	void GenerateSymmetryH();
	void GenerateQuarryH();
	void GenerateBunkerH();
	void GenerateSwampH();
	void GenerateTreehouseH();
	void GenerateTownH();
	void GenerateVaultsH();
	void GenerateTrianglePanelsH();
	void GenerateMountainH();
	void GenerateCavesH();

	//Environmental areas - unless I can figure out how to mess with the game's assets, randomizing some of these puzzles isn't happening anytime soon
	void GenerateOrchardH();
	void GenerateDesertH(); //Just scramble the positions for now
	void GenerateShadowsH(); //Can't randomize
	void GenerateKeepH();
	void GenerateMonasteryH(); //Can't randomize
	void GenerateJungleH();

private:
	std::shared_ptr<Generate> generator;
	HWND _handle;
};