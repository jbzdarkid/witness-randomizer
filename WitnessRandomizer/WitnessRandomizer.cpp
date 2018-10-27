/*
 * FEATURES:
 * SWAP_TARGETS should still require the full panel sequence (and have ways to prevent softlocks?)
 ** Think about: Jungle
 ** Hard: Monastery
 ** Do: Challenge
 * Randomize audio logs
 * Swap sounds in jungle (along with panels) -- maybe impossible
 * Make orange 7 (all of oranges?) hard. Like big = hard.
*/
#include "Memory.h"
#include "WitnessRandomizer.h"
#include "Panels.h"
#include <string>
#include <iostream>
#include <numeric>

template <class T>
int find(const std::vector<T> &data, T search, int startIndex = 0) {
	for (int i=startIndex ; i<data.size(); i++) {
		if (data[i] == search) return i;
	}
	return -1;
}

int main(int argc, char** argv)
{
	WitnessRandomizer randomizer = WitnessRandomizer();

	if (argc == 2) {
		srand(atoi(argv[1])); // Seed with RNG from command line
	} else {
		int seed = rand() % (1 << 16);
		std::cout << "Selected seed: " << seed << std::endl;
		srand(seed);
	}

	// Content swaps -- must happen before squarePanels
	randomizer.Randomize(upDownPanels, SWAP_LINES | SWAP_STYLE);
	randomizer.Randomize(leftForwardRightPanels, SWAP_LINES);

	randomizer.Randomize(squarePanels, SWAP_LINES | SWAP_STYLE);

	// Frame swaps -- must happen after squarePanels
	randomizer.Randomize(burnablePanels, SWAP_LINES | SWAP_STYLE);

	// Target swaps, can happen whenever
	randomizer.Randomize(lasers, SWAP_TARGETS);
	// Read the target of keep front laser, and write it to keep back laser.
	std::vector<int> keepFrontLaserTarget = randomizer.ReadPanelData<int>(0x0360E, TARGET, 1);
	randomizer.WritePanelData<int>(0x03317, TARGET, keepFrontLaserTarget);

	std::vector<int> randomOrder = std::vector(junglePanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	// Randomize Waves 2-7
	// Waves 1 cannot be randomized, since no other panel can start on
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 1, 7);
	// Randomize Pitches 1-6 onto themselves
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 7, 13);
	randomizer.ReassignTargets(junglePanels, randomOrder);

	randomOrder = std::vector(bunkerPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	// Randomize Tutorial 2-Advanced Tutorial 4 + Glass 1
	// Tutorial 1 cannot be randomized, since no other panel can start on
	// Glass 1 will become door + glass 1, due to the targetting system
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 1, 10);
	// Randomize Glass 1-3 into everything after the door
	int glassDoorIndex = find(randomOrder, 9) + 1;
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, glassDoorIndex, 12);
	randomizer.ReassignTargets(bunkerPanels, randomOrder);

	randomOrder = std::vector(shadowsPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 0, 8); // Tutorial
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 8, 16); // Avoid
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 16, 21); // Follow
	randomizer.ReassignTargets(shadowsPanels, randomOrder);

	// Turn off original starting panel
	randomizer.WritePanelData<float>(shadowsPanels[0], POWER, {0.0f, 0.0f});
	// Turn on new starting panel
	randomizer.WritePanelData<float>(shadowsPanels[randomOrder[0]], POWER, {1.0f, 1.0f});
}

WitnessRandomizer::WitnessRandomizer()
{
	// Turn off desert flood final
	WritePanelData<float>(0x18076, POWER, {0.0f, 0.0f});
	// Change desert floating target to desert flood final
	WritePanelData<int>(0x17ECA, TARGET, {0x18077});

	// Distance-gate shadows laser to prevent sniping through the bars
	WritePanelData<float>(0x19650, MAX_BROADCAST_DISTANCE, {2.5f});
	// Change the shadows tutorial cable to only activate avoid
	WritePanelData<int>(0x319A8, 0xD8, {0});
	// Change shadows avoid 8 to power shadows follow
	WritePanelData<int>(0x1972F, TARGET, {0x1C34C});

	// Disable tutorial cursor speed modifications
	WritePanelData<float>(0x00295, CURSOR_SPEED_SCALE, {1.0});
	WritePanelData<float>(0x0C373, CURSOR_SPEED_SCALE, {1.0});
	WritePanelData<float>(0x00293, CURSOR_SPEED_SCALE, {1.0});
	WritePanelData<float>(0x002C2, CURSOR_SPEED_SCALE, {1.0});
}

void WitnessRandomizer::Randomize(std::vector<int> &panels, int flags) {
	return RandomizeRange(panels, flags, 0, panels.size());
}

// Range is [start, end)
void WitnessRandomizer::RandomizeRange(std::vector<int> &panels, int flags, size_t startIndex, size_t endIndex) {
	if (panels.size() == 0) return;
	if (startIndex >= endIndex) return;
	if (endIndex >= panels.size()) endIndex = panels.size();
	for (size_t i = endIndex-1; i > startIndex+1; i--) {
		size_t target = rand() % (i - startIndex) + startIndex;
		if (i != target) {
			// std::cout << "Swapping panels " << std::hex << panels[i] << " and " << std::hex << panels[target] << std::endl;
			SwapPanels(panels[i], panels[target], flags);
			std::swap(panels[i], panels[target]); // Panel indices in the array
		}
	}
}

void WitnessRandomizer::SwapPanels(int panel1, int panel2, int flags) {
	std::map<int, int> offsets;

	if (flags & SWAP_TARGETS) {
		offsets[TARGET] = sizeof(int);
	}
	if (flags & SWAP_STYLE) {
		offsets[STYLE_FLAGS] = sizeof(int);
	}
	if (flags & SWAP_LINES) {
		offsets[PATH_COLOR] = 16;
		offsets[REFLECTION_PATH_COLOR] = 16;
		offsets[DOT_COLOR] = 16;
		offsets[ACTIVE_COLOR] = 16;
		offsets[BACKGROUND_REGION_COLOR] = 16;
		offsets[SUCCESS_COLOR_A] = 16;
		offsets[SUCCESS_COLOR_B] = 16;
		offsets[STROBE_COLOR_A] = 16;
		offsets[STROBE_COLOR_B] = 16;
		offsets[ERROR_COLOR] = 16;
		offsets[PATTERN_POINT_COLOR] = 16;
		offsets[PATTERN_POINT_COLOR_A] = 16;
		offsets[PATTERN_POINT_COLOR_B] = 16;
		offsets[SYMBOL_A] = 16;
		offsets[SYMBOL_B] = 16;
		offsets[SYMBOL_C] = 16;
		offsets[SYMBOL_D] = 16;
		offsets[SYMBOL_E] = 16;
		offsets[PUSH_SYMBOL_COLORS] = sizeof(int);
		offsets[OUTER_BACKGROUND] = 16;
		offsets[OUTER_BACKGROUND_MODE] = sizeof(int);
		offsets[TRACED_EDGES] = 16;
		offsets[AUDIO_PREFIX] = sizeof(void*);
//		offsets[IS_CYLINDER] = sizeof(int);
//		offsets[CYLINDER_Z0] = sizeof(float);
//		offsets[CYLINDER_Z1] = sizeof(float);
//		offsets[CYLINDER_RADIUS] = sizeof(float);
		offsets[SPECULAR_ADD] = sizeof(float);
		offsets[SPECULAR_POWER] = sizeof(int);
		offsets[PATH_WIDTH_SCALE] = sizeof(float);
		offsets[STARTPOINT_SCALE] = sizeof(float);
		offsets[NUM_DOTS] = sizeof(int);
		offsets[NUM_CONNECTIONS] = sizeof(int);
		offsets[DOT_POSITIONS] = sizeof(void*);
		offsets[DOT_FLAGS] = sizeof(void*);
		offsets[DOT_CONNECTION_A] = sizeof(void*);
		offsets[DOT_CONNECTION_B] = sizeof(void*);
		offsets[DECORATIONS] = sizeof(void*);
		offsets[DECORATION_FLAGS] = sizeof(void*);
		offsets[DECORATION_COLORS] = sizeof(void*);
		offsets[NUM_DECORATIONS] = sizeof(int);
		offsets[REFLECTION_DATA] = sizeof(void*);
		offsets[GRID_SIZE_X] = sizeof(int);
		offsets[GRID_SIZE_Y] = sizeof(int);
		offsets[SEQUENCE_LEN] = sizeof(int);
		offsets[SEQUENCE] = sizeof(void*);
		offsets[DOT_SEQUENCE_LEN] = sizeof(int);
		offsets[DOT_SEQUENCE] = sizeof(void*);
		offsets[DOT_SEQUENCE_LEN_REFLECTION] = sizeof(int);
		offsets[DOT_SEQUENCE_REFLECTION] = sizeof(void*);
		offsets[NUM_COLORED_REGIONS] = sizeof(int);
		offsets[COLORED_REGIONS] = sizeof(void*);
		offsets[PANEL_TARGET] = sizeof(void*);
		offsets[SPECULAR_TEXTURE] = sizeof(void*);
	}

	for (auto const& [offset, size] : offsets) {
		std::vector<byte> panel1data = ReadPanelData<byte>(panel1, offset, size);
		std::vector<byte> panel2data = ReadPanelData<byte>(panel2, offset, size);
		WritePanelData<byte>(panel2, offset, panel1data);
		WritePanelData<byte>(panel1, offset, panel2data);
	}
}

void WitnessRandomizer::ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order) {
	std::vector<int> targetToActivatePanel = {panels[0] + 1};
	for (int panel : panels) {
		int target = ReadPanelData<int>(panel, TARGET, 1)[0];
		targetToActivatePanel.push_back(target);
	}

	for (int i=0; i<order.size() - 1; i++) {
		// order[i+1] is the target panel
		// order[i+1] - 1 is the (real) panel before the target panel
		// targets[order[i+1] - 1] is the (real) target which will activate the target panel
		int panelTarget = targetToActivatePanel[order[i+1]];
		WritePanelData<int>(panels[order[i]], TARGET, {panelTarget});
	}
}
