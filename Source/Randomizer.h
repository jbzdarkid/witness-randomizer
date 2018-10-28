#pragma once
#include "RandomizerCore.h"

class Randomizer {
public:
	void Randomize();

private:
	RandomizerCore _core;
	void RandomizeTutorial();
	void RandomizeSymmetry();
	void RandomizeDesert();
	void RandomizeQuarry();
	void RandomizeTreehouse();
	void RandomizeKeep();
	void RandomizeShadows();
	void RandomizeTown();
	void RandomizeMonastery();
	void RandomizeBunker();
	void RandomizeJungle();
	void RandomizeSwamp();
	void RandomizeMountain();
};
