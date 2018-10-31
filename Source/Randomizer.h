#pragma once
#include "RandomizerCore.h"

class Randomizer {
public:
	short Randomize(int seed);
	void AdjustSpeed();

private:
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
	void RandomizeChallenge();
	void RandomizeAudioLogs();

	RandomizerCore _core;
};
