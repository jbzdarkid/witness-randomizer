#pragma once
#include "RandomizerCore.h"

class Randomizer {
public:
	void Randomize();
	bool GameIsRandomized();

	void AdjustSpeed();

	void ClearOffsets() {_core.ClearOffsets();}

private:

	int _lastRandomizedFrame = 1 << 30;
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
