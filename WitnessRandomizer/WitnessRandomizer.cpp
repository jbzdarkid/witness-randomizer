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
		int seed = rand() % 1 << 16;
		std::cout << "Selected seed:" << seed << std::endl;
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
	std::vector<int> keepFrontLaserTarget = randomizer.ReadPanelData<int>(0x0360E, 0x2BC, 1);
	randomizer.WritePanelData<int>(0x03317, 0x2BC, keepFrontLaserTarget);

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
	randomizer.WritePanelData<float>(shadowsPanels[0], 0x2A8, {0.0f, 0.0f});
	// Turn on new starting panel
	randomizer.WritePanelData<float>(shadowsPanels[randomOrder[0]], 0x2A8, {1.0f, 1.0f});
}

WitnessRandomizer::WitnessRandomizer() : _memory("witness64_d3d11.exe")
{
	// Turn off desert flood final
	WritePanelData<float>(0x18076, 0x2A8, {0.0f, 0.0f});
	// Change desert floating target to desert flood final
	WritePanelData<int>(0x17ECA, 0x2BC, {0x18077});

	// Distance-gate shadows laser to prevent sniping through the bars
	WritePanelData<float>(0x19650, 0x3C0, {2.5f});
	// Change the shadows tutorial cable to only activate avoid
	WritePanelData<int>(0x319A8, 0xD8, {0});
	// Change shadows avoid 8 to power shadows follow
	WritePanelData<int>(0x1972F, 0x2BC, {0x1C34C});

	// Disable tutorial cursor speed modifications
	WritePanelData<float>(0x00295, 0x358, {1.0});
	WritePanelData<float>(0x0C373, 0x358, {1.0});
	WritePanelData<float>(0x00293, 0x358, {1.0});
	WritePanelData<float>(0x002C2, 0x358, {1.0});
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
		offsets[0x2BC] = sizeof(int);
	}
	if (flags & SWAP_STYLE) {
		offsets[0x450] = sizeof(int); // style_flags
	}
	if (flags & SWAP_LINES) {
		offsets[0xC8] = 16; // path_color
		offsets[0xD8] = 16; // reflection_path_color
		offsets[0xF8] = 16; // dot_color
		offsets[0x108] = 16; // active_color
		offsets[0x118] = 16; // background_region_color
		offsets[0x128] = 16; // success_color_a
		offsets[0x138] = 16; // success_color_b
		offsets[0x148] = 16; // strobe_color_a
		offsets[0x158] = 16; // strobe_color_b
		offsets[0x168] = 16; // error_color
		offsets[0x188] = 16; // pattern_point_color
		offsets[0x198] = 16; // pattern_point_color_a
		offsets[0x1A8] = 16; // pattern_point_color_b
		offsets[0x1B8] = 16; // symbol_a
		offsets[0x1C8] = 16; // symbol_b
		offsets[0x1D8] = 16; // symbol_c
		offsets[0x1E8] = 16; // symbol_d
		offsets[0x1F8] = 16; // symbol_e
		offsets[0x208] = sizeof(int); // push_symbol_colors
		offsets[0x20C] = 16; // outer_background
		offsets[0x21C] = sizeof(int); // outer_background_mode
		offsets[0x230] = 16; // traced_edges
		offsets[0x278] = sizeof(void*); // *audio_prefix
		offsets[0x2FC] = sizeof(int); // is_cylinder
		offsets[0x300] = sizeof(float); // cylinder_z0
		offsets[0x304] = sizeof(float); // cylinder_z1
		offsets[0x308] = sizeof(float); // cylinder_radius
		offsets[0x398] = sizeof(float); // specular_add
		offsets[0x39C] = sizeof(int); // specular_power
		offsets[0x3A4] = sizeof(float); // path_width_scale
		offsets[0x3A8] = sizeof(float); // startpoint_scale
		offsets[0x3B8] = sizeof(int); // num_dots
		offsets[0x3BC] = sizeof(int); // num_connections
		offsets[0x3C8] = sizeof(void*); // *dot_positions
		offsets[0x3D0] = sizeof(void*); // *dot_flags
		offsets[0x3D8] = sizeof(void*); // *dot_connection_a
		offsets[0x3E0] = sizeof(void*); // *dot_connection_b
		offsets[0x420] = sizeof(void*); // *decorations
		offsets[0x428] = sizeof(void*); // *decoration_flags
		offsets[0x430] = sizeof(void*); // *decoration_colors
		offsets[0x438] = sizeof(int); // num_decorations
		offsets[0x440] = sizeof(void*); // *reflection_data
		offsets[0x448] = sizeof(int); // grid_size_x
		offsets[0x44C] = sizeof(int); // grid_size_y
		offsets[0x45C] = sizeof(int); // sequence_len
		offsets[0x460] = sizeof(void*); // *sequence
		offsets[0x468] = sizeof(int); // dot_sequence_len
		offsets[0x470] = sizeof(void*); // *dot_sequence
		offsets[0x478] = sizeof(int); // dot_sequence_len_reflection
		offsets[0x480] = sizeof(void*); // *dot_sequence_reflection
		offsets[0x4A0] = sizeof(int); // num_colored_regions
		offsets[0x4A8] = sizeof(void*); // *colored_regions
		offsets[0x4B0] = sizeof(void*); // *panel_target
		offsets[0x4D8] = sizeof(void*); // *specular_texture
	}

	for (auto const& [offset, size] : offsets) {
		std::vector<byte> data = ReadPanelData<byte>(panel1, offset, size);
		WritePanelData<byte>(panel2, offset, data);
	}
}

void WitnessRandomizer::ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order) {
	std::vector<int> targetToActivatePanel = {panels[0] + 1};
	for (int panel : panels) {
		int target = ReadPanelData<int>(panel, 0x2BC, 1)[0];
		targetToActivatePanel.push_back(target);
	}

	for (int i=0; i<order.size() - 1; i++) {
		// order[i+1] is the target panel
		// order[i+1] - 1 is the (real) panel before the target panel
		// targets[order[i+1] - 1] is the (real) target which will activate the target panel
		int panelTarget = targetToActivatePanel[order[i+1]];
		WritePanelData<int>(panels[order[i]], 0x2BC, {panelTarget});
	}
}
