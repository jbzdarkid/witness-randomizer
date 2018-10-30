/*
 * BUGS:
 * Shipwreck vault is solved reversed?
 * Swamp <-> symmetry has non-invisible background
 * Tutorial sounds don't always play
 * FEATURES:
 * Speed up some of the slow things (like swamp)
 * Prevent re-randomization (?)
 * Clear "Randomized" state on NG (?) -- Or on a short delay
 * Randomize audio logs -- Hard, seem to be unloaded some times?
 * Swap sounds in jungle (along with panels) -- maybe impossible
 * Make orange 7 (all of oranges?) hard. Like big = hard.
 * Start the game if it isn't running?
 * SETTINGS:
 * Speeds of certain things
 * Speedrunner mode
 * Increase odds of mountain oranges garbage on other panels
*/
#include "Memory.h"
#include "Randomizer.h"
#include "Panels.h"
#include <string>
#include <iostream>
#include <numeric>
#include <chrono>

template <class T>
int find(const std::vector<T> &data, T search, size_t startIndex = 0) {
	for (size_t i=startIndex ; i<data.size(); i++) {
		if (data[i] == search) return static_cast<int>(i);
	}
	std::cout << "Couldn't find " << search << " in data!" << std::endl;
	exit(-1);
}

void Randomizer::Randomize()
{
	// Content swaps -- must happen before squarePanels
	_core.Randomize(upDownPanels, SWAP_LINES);
	_core.Randomize(leftForwardRightPanels, SWAP_LINES);

	_core.Randomize(squarePanels, SWAP_LINES);

	// Individual area modifications
	RandomizeTutorial();
	RandomizeSymmetry();
	RandomizeDesert();
	RandomizeQuarry();
	RandomizeTreehouse();
	RandomizeKeep();
	RandomizeShadows();
	RandomizeTown();
	RandomizeMonastery();
	RandomizeBunker();
	RandomizeJungle();
	RandomizeSwamp();
	RandomizeMountain();
	// RandomizeChallenge();
	// RandomizeAudioLogs();
}

void Randomizer::AdjustSpeed() {
	// Desert Surface Final Control
	_core.WritePanelData<float>(0x09F95, OPEN_RATE, {0.04}); // 4x
	// Swamp Sliding Bridge
	_core.WritePanelData<float>(0x0061A, OPEN_RATE, {0.1}); // 4x
	// Mountain 2 Elevator
	_core.WritePanelData<float>(0x09EEC, OPEN_RATE, {0.1}); // 4x
}

void Randomizer::RandomizeTutorial() {
	// Disable tutorial cursor speed modifications (not working?)
	_core.WritePanelData<float>(0x00295, CURSOR_SPEED_SCALE, {1.0});
	_core.WritePanelData<float>(0x0C373, CURSOR_SPEED_SCALE, {1.0});
	_core.WritePanelData<float>(0x00293, CURSOR_SPEED_SCALE, {1.0});
	_core.WritePanelData<float>(0x002C2, CURSOR_SPEED_SCALE, {1.0});
}

void Randomizer::RandomizeSymmetry() {
}

void Randomizer::RandomizeDesert() {
	_core.Randomize(desertPanels, SWAP_LINES);

	// Turn off desert surface 8
	_core.WritePanelData<float>(0x09F94, POWER, {0.0, 0.0});
	// Turn off desert flood final
	_core.WritePanelData<float>(0x18076, POWER, {0.0, 0.0});
	// Change desert floating target to desert flood final
	_core.WritePanelData<int>(0x17ECA, TARGET, {0x18077});
}

void Randomizer::RandomizeQuarry() {
}

void Randomizer::RandomizeTreehouse() {
	// Ensure that whatever pivot panels we have are flagged as "pivotable"
	int panelFlags = _core.ReadPanelData<int>(0x17DD1, STYLE_FLAGS, 1)[0];
	_core.WritePanelData<int>(0x17DD1, STYLE_FLAGS, {panelFlags | 0x8000});
	panelFlags = _core.ReadPanelData<int>(0x17CE3, STYLE_FLAGS, 1)[0];
	_core.WritePanelData<int>(0x17CE3, STYLE_FLAGS, {panelFlags | 0x8000});
	panelFlags = _core.ReadPanelData<int>(0x17DB7, STYLE_FLAGS, 1)[0];
	_core.WritePanelData<int>(0x17DB7, STYLE_FLAGS, {panelFlags | 0x8000});
	panelFlags = _core.ReadPanelData<int>(0x17E52, STYLE_FLAGS, 1)[0];
	_core.WritePanelData<int>(0x17E52, STYLE_FLAGS, {panelFlags | 0x8000});
}

void Randomizer::RandomizeKeep() {
}

void Randomizer::RandomizeShadows() {
	// Distance-gate shadows laser to prevent sniping through the bars
	_core.WritePanelData<float>(0x19650, MAX_BROADCAST_DISTANCE, {2.5});
	// Change the shadows tutorial cable to only activate avoid
	_core.WritePanelData<int>(0x319A8, CABLE_TARGET_2, {0});
	// Change shadows avoid 8 to power shadows follow
	_core.WritePanelData<int>(0x1972F, TARGET, {0x1C34C});

	std::vector<int> randomOrder(shadowsPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	_core.RandomizeRange(randomOrder, SWAP_NONE, 0, 8); // Tutorial
	_core.RandomizeRange(randomOrder, SWAP_NONE, 8, 16); // Avoid
	_core.RandomizeRange(randomOrder, SWAP_NONE, 16, 21); // Follow
	_core.ReassignTargets(shadowsPanels, randomOrder);
	// Turn off original starting panel
	_core.WritePanelData<float>(shadowsPanels[0], POWER, {0.0f, 0.0f});
	// Turn on new starting panel
	_core.WritePanelData<float>(shadowsPanels[randomOrder[0]], POWER, {1.0f, 1.0f});
}

void Randomizer::RandomizeTown() {
}

void Randomizer::RandomizeMonastery() {
	std::vector<int> randomOrder(monasteryPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	_core.RandomizeRange(randomOrder, SWAP_NONE, 3, 9); // Outer 2 & 3, Inner 1-4
	_core.ReassignTargets(monasteryPanels, randomOrder);
}

void Randomizer::RandomizeBunker() {
	std::vector<int> randomOrder(bunkerPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	// Randomize Tutorial 2-Advanced Tutorial 4 + Glass 1
	// Tutorial 1 cannot be randomized, since no other panel can start on
	// Glass 1 will become door + glass 1, due to the targetting system
	_core.RandomizeRange(randomOrder, SWAP_NONE, 1, 10);
	// Randomize Glass 1-3 into everything after the door/glass 1
	const size_t glass1Index = find(randomOrder, 9);
	_core.RandomizeRange(randomOrder, SWAP_NONE, glass1Index + 1, 12);
	_core.ReassignTargets(bunkerPanels, randomOrder);
}

void Randomizer::RandomizeJungle() {
	std::vector<int> randomOrder(junglePanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	// Waves 1 cannot be randomized, since no other panel can start on
	_core.RandomizeRange(randomOrder, SWAP_NONE, 1, 7); // Waves 2-7
	_core.RandomizeRange(randomOrder, SWAP_NONE, 8, 13); // Pitches 1-6
	_core.ReassignTargets(junglePanels, randomOrder);
}

void Randomizer::RandomizeSwamp() {
	// Distance-gate swamp snipe 1 to prevent RNG swamp snipe
	_core.WritePanelData<float>(0x17C05, MAX_BROADCAST_DISTANCE, {15.0});
}

void Randomizer::RandomizeMountain() {
	// Randomize lasers & some of mountain
	_core.Randomize(lasers, SWAP_TARGETS);
	_core.Randomize(mountainMultipanel, SWAP_LINES);

	// Randomize final pillars order
	std::vector<int> targets = {pillars[0] + 1};
	for (const int pillar : pillars) {
		int target = _core.ReadPanelData<int>(pillar, TARGET, 1)[0];
		targets.push_back(target);
	}
	targets[5] = pillars[5] + 1;

	std::vector<int> randomOrder(pillars.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	_core.RandomizeRange(randomOrder, SWAP_NONE, 0, 4); // Left Pillars 1-4
	_core.RandomizeRange(randomOrder, SWAP_NONE, 5, 9); // Right Pillars 1-4
	_core.ReassignTargets(pillars, randomOrder, targets);
	// Turn off original starting panels
	_core.WritePanelData<float>(pillars[0], POWER, {0.0f, 0.0f});
	_core.WritePanelData<float>(pillars[5], POWER, {0.0f, 0.0f});
	// Turn on new starting panels
	_core.WritePanelData<float>(pillars[randomOrder[0]], POWER, {1.0f, 1.0f});
	_core.WritePanelData<float>(pillars[randomOrder[5]], POWER, {1.0f, 1.0f});

	// Read the target of keep front laser, and write it to keep back laser.
	std::vector<int> keepFrontLaserTarget = _core.ReadPanelData<int>(0x0360E, TARGET, 1);
	_core.WritePanelData<int>(0x03317, TARGET, keepFrontLaserTarget);
}

void Randomizer::RandomizeChallenge() {
	std::vector<int> randomOrder(challengePanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	_core.RandomizeRange(randomOrder, SWAP_NONE, 1, 9); // Easy maze - Triple 2
	std::vector<int> triple1Target = _core.ReadPanelData<int>(0x00C80, TARGET, 1);
	_core.WritePanelData<int>(0x00CA1, TARGET, triple1Target);
	_core.WritePanelData<int>(0x00CB9, TARGET, triple1Target);
	std::vector<int> triple2Target = _core.ReadPanelData<int>(0x00C22, TARGET, 1);
	_core.WritePanelData<int>(0x00C59, TARGET, triple2Target);
	_core.WritePanelData<int>(0x00C68, TARGET, triple2Target);
	_core.ReassignTargets(challengePanels, randomOrder);
}

void Randomizer::RandomizeAudioLogs() {
	std::vector<int> randomOrder(audiologs.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	_core.Randomize(randomOrder, SWAP_NONE);
	_core.ReassignNames(audiologs, randomOrder);
}