#pragma once
#include "Memory.h"
#include <memory>

class Randomizer {
public:
	void Randomize();
	bool GameIsRandomized();

	void AdjustSpeed();
	void RandomizeLasers();
	void PreventSnipes();

	void ClearOffsets() {_memory->ClearOffsets();}

	enum SWAP {
		NONE = 0,
		TARGETS = 1,
		LINES = 2,
		AUDIO_NAMES = 4,
		COLORS = 8,
	};

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

	std::shared_ptr<Memory> _memory = std::make_shared<Memory>("witness64_d3d11.exe");

	friend class SwapTests_Shipwreck_Test;
};

#if GLOBALS == 0x5B28C0
#define PATH_COLOR 0xC8
#define REFLECTION_PATH_COLOR 0xD8
#define DOT_COLOR 0xF8
#define ACTIVE_COLOR 0x108
#define BACKGROUND_REGION_COLOR 0x118
#define SUCCESS_COLOR_A 0x128
#define SUCCESS_COLOR_B 0x138
#define STROBE_COLOR_A 0x148
#define STROBE_COLOR_B 0x158
#define ERROR_COLOR 0x168
#define PATTERN_POINT_COLOR 0x188
#define PATTERN_POINT_COLOR_A 0x198
#define PATTERN_POINT_COLOR_B 0x1A8
#define SYMBOL_A 0x1B8
#define SYMBOL_B 0x1C8
#define SYMBOL_C 0x1D8
#define SYMBOL_D 0x1E8
#define SYMBOL_E 0x1F8
#define PUSH_SYMBOL_COLORS 0x208
#define OUTER_BACKGROUND 0x20C
#define OUTER_BACKGROUND_MODE 0x21C
#define TRACED_EDGES 0x230
#define AUDIO_PREFIX 0x278
#define POWER 0x2A8
#define TARGET 0x2BC
#define POWER_OFF_ON_FAIL 0x2C0
#define IS_CYLINDER 0x2FC
#define CYLINDER_Z0 0x300
#define CYLINDER_Z1 0x304
#define CYLINDER_RADIUS 0x308
#define CURSOR_SPEED_SCALE 0x358
#define NEEDS_REDRAW 0x384
#define SPECULAR_ADD 0x398
#define SPECULAR_POWER 0x39C
#define PATH_WIDTH_SCALE 0x3A4
#define STARTPOINT_SCALE 0x3A8
#define NUM_DOTS 0x3B8
#define NUM_CONNECTIONS 0x3BC
#define MAX_BROADCAST_DISTANCE 0x3C0
#define DOT_POSITIONS 0x3C8
#define DOT_FLAGS 0x3D0
#define DOT_CONNECTION_A 0x3D8
#define DOT_CONNECTION_B 0x3E0
#define DECORATIONS 0x420
#define DECORATION_FLAGS 0x428
#define DECORATION_COLORS 0x430
#define NUM_DECORATIONS 0x438
#define REFLECTION_DATA 0x440
#define GRID_SIZE_X 0x448
#define GRID_SIZE_Y 0x44C
#define STYLE_FLAGS 0x450
#define SEQUENCE_LEN 0x45C
#define SEQUENCE 0x460
#define DOT_SEQUENCE_LEN 0x468
#define DOT_SEQUENCE 0x470
#define DOT_SEQUENCE_LEN_REFLECTION 0x478
#define DOT_SEQUENCE_REFLECTION 0x480
#define NUM_COLORED_REGIONS 0x4A0
#define COLORED_REGIONS 0x4A8
#define PANEL_TARGET 0x4B0
#define SPECULAR_TEXTURE 0x4D8
#define CABLE_TARGET_2 0xD8
#define AUDIO_LOG_NAME 0xC8
#define OPEN_RATE 0xE8
#define METADATA 0xF2 // sizeof(short)
#define HOTEL_EP_NAME 0x4BC640
#elif GLOBALS == 0x62A080
#define PATH_COLOR 0xC0
#define REFLECTION_PATH_COLOR 0xD0
#define DOT_COLOR 0xF0
#define ACTIVE_COLOR 0x100
#define BACKGROUND_REGION_COLOR 0x110
#define SUCCESS_COLOR_A 0x120
#define SUCCESS_COLOR_B 0x130
#define STROBE_COLOR_A 0x140
#define STROBE_COLOR_B 0x150
#define ERROR_COLOR 0x160
#define PATTERN_POINT_COLOR 0x180
#define PATTERN_POINT_COLOR_A 0x190
#define PATTERN_POINT_COLOR_B 0x1A0
#define SYMBOL_A 0x1B0
#define SYMBOL_B 0x1C0
#define SYMBOL_C 0x1D0
#define SYMBOL_D 0x1E0
#define SYMBOL_E 0x1F0
#define PUSH_SYMBOL_COLORS 0x200
#define OUTER_BACKGROUND 0x204
#define OUTER_BACKGROUND_MODE 0x214
#define TRACED_EDGES 0x228
#define AUDIO_PREFIX 0x270
#define POWER 0x2A0
#define TARGET 0x2B4
#define POWER_OFF_ON_FAIL 0x2B8
#define IS_CYLINDER 0x2F4
#define CYLINDER_Z0 0x2F8
#define CYLINDER_Z1 0x2FC
#define CYLINDER_RADIUS 0x300
#define CURSOR_SPEED_SCALE 0x350
#define NEEDS_REDRAW 0x37C
#define SPECULAR_ADD 0x38C
#define SPECULAR_POWER 0x390
#define PATH_WIDTH_SCALE 0x39C
#define STARTPOINT_SCALE 0x3A0
#define NUM_DOTS 0x3B4
#define NUM_CONNECTIONS 0x3B8
#define MAX_BROADCAST_DISTANCE 0x3BC
#define DOT_POSITIONS 0x3C0
#define DOT_FLAGS 0x3C8
#define DOT_CONNECTION_A 0x3D0
#define DOT_CONNECTION_B 0x3D8
#define DECORATIONS 0x418
#define DECORATION_FLAGS 0x420
#define DECORATION_COLORS 0x428
#define NUM_DECORATIONS 0x430
#define REFLECTION_DATA 0x438
#define GRID_SIZE_X 0x440
#define GRID_SIZE_Y 0x444
#define STYLE_FLAGS 0x448
#define SEQUENCE_LEN 0x454
#define SEQUENCE 0x458
#define DOT_SEQUENCE_LEN 0x460
#define DOT_SEQUENCE 0x468
#define DOT_SEQUENCE_LEN_REFLECTION 0x470
#define DOT_SEQUENCE_REFLECTION 0x478
#define NUM_COLORED_REGIONS 0x498
#define COLORED_REGIONS 0x4A0
#define PANEL_TARGET 0x4A8
#define SPECULAR_TEXTURE 0x4D0
#define CABLE_TARGET_2 0xD0
#define AUDIO_LOG_NAME 0x0
#define OPEN_RATE 0xE0
#define METADATA 0x13A // sizeof(short)
#define HOTEL_EP_NAME 0x51E340
#endif