/*
 * TODO: Split out main() logic into another file, and move into separate functions for easier testing. Then write tests.
 * BUGS:
 * Shipwreck vault fails, possibly because of dot_reflection? Sometimes?
 * Treehouse pivots *should* work, but I need to not copy style_flags.
   This seems to cause crashes when pivots appear elsewhere in the world.
 * Some panels are impossible casually: (idc, I think)
 ** Town Stars, Invisible dots
 * Something is wrong with jungle re: softlocks
 * FEATURES:
 * SWAP_TARGETS should still require the full panel sequence (and have ways to prevent softlocks?)
 ** Think about: Jungle
 ** Hard: Monastery
 ** Do: Challenge
 * Randomize audio logs
 * Swap sounds in jungle (along with panels) -- maybe impossible
 * Make orange 7 (all of oranges?) hard. Like big = hard.
 * Start the game if it isn't running?
 * UI for the randomizer :(
 * Increase odds of mountain oranges garbage on other panels?
*/
#include "Memory.h"
#include "Randomizer.h"
#include "Panels.h"
#include <string>
#include <iostream>
#include <numeric>
#include <chrono>

template <class T>
size_t find(const std::vector<T> &data, T search, size_t startIndex = 0) {
	for (size_t i=startIndex ; i<data.size(); i++) {
		if (data[i] == search) return i;
	}
	std::cout << "Couldn't find " << search << " in data!" << std::endl;
	exit(-1);
}

void Randomizer::Randomize(int seed)
{
	// Content swaps -- must happen before squarePanels
	Randomize(tallUpDownPanels, SWAP_LINES | SWAP_STYLE);
	Randomize(upDownPanels, SWAP_LINES | SWAP_STYLE);
	Randomize(leftForwardRightPanels, SWAP_LINES);

	Randomize(squarePanels, SWAP_LINES | SWAP_STYLE);

	// Frame swaps -- must happen after squarePanels
	Randomize(burnablePanels, SWAP_LINES | SWAP_STYLE);

	// Target swaps, can happen whenever
	Randomize(lasers, SWAP_TARGETS);
	// Read the target of keep front laser, and write it to keep back laser.
	std::vector<int> keepFrontLaserTarget = ReadPanelData<int>(0x0360E, TARGET, 1);
	WritePanelData<int>(0x03317, TARGET, keepFrontLaserTarget);

	std::vector<int> randomOrder;

	/* Jungle
	randomOrder = std::vector(junglePanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	// Randomize Waves 2-7
	// Waves 1 cannot be randomized, since no other panel can start on
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 1, 7);
	// Randomize Pitches 1-6 onto themselves
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 7, 13);
	randomizer.ReassignTargets(junglePanels, randomOrder);
	*/

	/* Bunker */
	randomOrder = std::vector(bunkerPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	// Randomize Tutorial 2-Advanced Tutorial 4 + Glass 1
	// Tutorial 1 cannot be randomized, since no other panel can start on
	// Glass 1 will become door + glass 1, due to the targetting system
	RandomizeRange(randomOrder, SWAP_NONE, 1, 10);
	// Randomize Glass 1-3 into everything after the door
	const size_t glassDoorIndex = find(randomOrder, 9) + 1;
	RandomizeRange(randomOrder, SWAP_NONE, glassDoorIndex, 12);
	ReassignTargets(bunkerPanels, randomOrder);

	/* Shadows */
	randomOrder = std::vector(shadowsPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	RandomizeRange(randomOrder, SWAP_NONE, 0, 8); // Tutorial
	RandomizeRange(randomOrder, SWAP_NONE, 8, 16); // Avoid
	RandomizeRange(randomOrder, SWAP_NONE, 16, 21); // Follow
	ReassignTargets(shadowsPanels, randomOrder);
	// Turn off original starting panel
	WritePanelData<float>(shadowsPanels[0], POWER, {0.0f, 0.0f});
	// Turn on new starting panel
	WritePanelData<float>(shadowsPanels[randomOrder[0]], POWER, {1.0f, 1.0f});

	/* Monastery
	randomOrder = std::vector(monasteryPanels.size(), 0);
	std::iota(randomOrder.begin(), randomOrder.end(), 0);
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, 2, 6); // outer 2 & 3, inner 1
	// Once outer 3 and right door are solved, inner 2-4 are accessible
	int innerPanelsIndex = max(find(randomOrder, 2), find(randomOrder, 4));
	randomizer.RandomizeRange(randomOrder, SWAP_NONE, innerPanelsIndex, 9); // Inner 2-4

	randomizer.ReassignTargets(monasteryPanels, randomOrder);
	*/
}

Randomizer::Randomizer()
{
	// Turn off desert surface 8
	WritePanelData<float>(0x09F94, POWER, {0.0, 0.0});
	// Turn off desert flood final
	WritePanelData<float>(0x18076, POWER, {0.0, 0.0});
	// Change desert floating target to desert flood final
	WritePanelData<int>(0x17ECA, TARGET, {0x18077});

	// Distance-gate shadows laser to prevent sniping through the bars
	WritePanelData<float>(0x19650, MAX_BROADCAST_DISTANCE, {2.5});
	// Change the shadows tutorial cable to only activate avoid
	WritePanelData<int>(0x319A8, CABLE_TARGET_2, {0});
	// Change shadows avoid 8 to power shadows follow
	WritePanelData<int>(0x1972F, TARGET, {0x1C34C});

	// Distance-gate swamp snipe 1 to prevent RNG swamp snipe
	WritePanelData<float>(0x17C05, MAX_BROADCAST_DISTANCE, {5.0});

	// Disable tutorial cursor speed modifications (not working?)
	WritePanelData<float>(0x00295, CURSOR_SPEED_SCALE, {1.0});
	WritePanelData<float>(0x0C373, CURSOR_SPEED_SCALE, {1.0});
	WritePanelData<float>(0x00293, CURSOR_SPEED_SCALE, {1.0});
	WritePanelData<float>(0x002C2, CURSOR_SPEED_SCALE, {1.0});
}

void Randomizer::Randomize(std::vector<int>& panels, int flags) {
	return RandomizeRange(panels, flags, 0, panels.size());
}

// Range is [start, end)
void Randomizer::RandomizeRange(std::vector<int> &panels, int flags, size_t startIndex, size_t endIndex) {
	if (panels.size() == 0) return;
	if (startIndex >= endIndex) return;
	if (endIndex >= panels.size()) endIndex = panels.size();
	for (size_t i = endIndex-1; i > startIndex+1; i--) {
		const size_t target = rand() % (i - startIndex) + startIndex;
		if (i != target) {
			// std::cout << "Swapping panels " << std::hex << panels[i] << " and " << std::hex << panels[target] << std::endl;
			SwapPanels(panels[i], panels[target], flags);
			std::swap(panels[i], panels[target]); // Panel indices in the array
		}
	}
}

void Randomizer::SwapPanels(int panel1, int panel2, int flags) {
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

void Randomizer::ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order) {
	// This list is offset by 1, so the target of the Nth panel is in position N (aka the N+1th element)
	// The first panel may not have a wire to power it, so we use the panel ID itself.
	std::vector<int> targetToActivatePanel = {panels[0] + 1};
	for (const int panel : panels) {
		int target = ReadPanelData<int>(panel, TARGET, 1)[0];
		targetToActivatePanel.push_back(target);
	}

	for (size_t i=0; i<order.size() - 1; i++) {
		// Set the target of order[i] to order[i+1], using the "real" target as determined above.
		const int panelTarget = targetToActivatePanel[order[i+1]];
		WritePanelData<int>(panels[order[i]], TARGET, {panelTarget});
	}
}
