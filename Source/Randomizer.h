#pragma once
#include "RandomizerCore.h"

class Randomizer {
public:
	void Randomize();
	bool GameIsRandomized();

	void AdjustSpeed();

	void ClearOffsets() {_memory->ClearOffsets();}

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

	void Randomize(std::vector<int>& panels, int flags);
	void RandomizeRange(std::vector<int> &panels, int flags, size_t startIndex, size_t endIndex);
	void SwapPanels(int panel1, int panel2, int flags);
	void ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order, std::vector<int> targets = {});
	void ReassignNames(const std::vector<int>& panels, const std::vector<int>& order);

	short ReadMetadata();
	void WriteMetadata(short metadata);
	int GetCurrentFrame();

	std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe");

	friend class SwapTests_Shipwreck_Test;
};
