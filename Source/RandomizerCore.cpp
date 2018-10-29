#include "RandomizerCore.h"
#include "Memory.h"
#include <sstream>

void RandomizerCore::Randomize(std::vector<int>& panels, int flags) {
	return RandomizeRange(panels, flags, 0, panels.size());
}

// Range is [start, end)
void RandomizerCore::RandomizeRange(std::vector<int> &panels, int flags, size_t startIndex, size_t endIndex) {
	if (panels.size() == 0) return;
	if (startIndex >= endIndex) return;
	if (endIndex >= panels.size()) endIndex = panels.size();
	for (size_t i = endIndex-1; i > startIndex; i--) {
		const size_t target = rand() % (i - startIndex) + startIndex;
		if (i != target) {
			// std::cout << "Swapping panels " << std::hex << panels[i] << " and " << std::hex << panels[target] << std::endl;
			SwapPanels(panels[i], panels[target], flags);
			std::swap(panels[i], panels[target]); // Panel indices in the array
		}
	}
}

void RandomizerCore::SwapPanels(int panel1, int panel2, int flags) {
	std::map<int, int> offsets;

	if (flags & SWAP_TARGETS) {
		offsets[TARGET] = sizeof(int);
	}
	if (flags & SWAP_AUDIO_NAMES) {
		offsets[AUDIO_LOG_NAME] = sizeof(void*);
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
		offsets[EXTRA_BACK_DISTANCE] = sizeof(float);
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
		offsets[STYLE_FLAGS] = sizeof(int);
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

void RandomizerCore::ReassignTargets(const std::vector<int>& panels, const std::vector<int>& order) {
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

void RandomizerCore::ReassignNames(const std::vector<int>& panels, const std::vector<int>& order) {
	std::vector<int64_t> names;
	for (const int panel : panels) {
		names.push_back(ReadPanelData<int64_t>(panel, AUDIO_LOG_NAME, 1)[0]);
	}

	for (int i=0; i<panels.size(); i++) {
		WritePanelData<int64_t>(panels[i], AUDIO_LOG_NAME, {names[order[i]]});
	}
}
