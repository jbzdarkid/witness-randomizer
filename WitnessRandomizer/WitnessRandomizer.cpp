/* BUGS:
 * Quarry colors are broken (specfically boathouse row 2 is translating wrong)
 * Desert flood exit isn't off properly
 * FEATURES:
 * Randomize audio logs
 * Randomize lasers
 * Tackle remaining areas (Treehouse, Keep, Shadows, Town, Monastery, Jungle, Bunker, Swamp)
 * Compile panel lists in a more reasonable way
 * Separate panels out by "square", which can probably be anywhere
 * List of panels which can be valid "Burn" (aka desert) substitutes: Prevent (or limit) panel counting
*/
#include "Memory.h"
#include "WitnessRandomizer.h"
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
	WitnessRandomizer randomizer = WitnessRandomizer();

	//*
	if (argc == 2) {
		srand(atoi(argv[1])); // Seed with RNG from command line
	}

	std::vector<int> tutorialPanels = {
		// 0x0A3B5, // Back Left
		0x0A3B2, // Back Right
		0x00295, // Center Left
		0x00293, // Front Center
		0x002C2, // Front Left
		0x0C335, // Pillar
		0x0C373, // Patio floor
	};

	randomizer.Randomize(tutorialPanels, SWAP_TARGETS);

	std::vector<int> symmetryPanels = {
//		0x3C12B, // Glass Factory Discard
		0x01A54, // Glass Factory Entry
		0x00086, // Glass Factory Vertical Symmetry 1
		0x00087, // Glass Factory Vertical Symmetry 2
		0x00059, // Glass Factory Vertical Symmetry 3
		0x00062, // Glass Factory Vertical Symmetry 4
		0x0005C, // Glass Factory Vertical Symmetry 5
		// 0x17CC8, // Glass Factory Summon Boat
		0x0008D, // Glass Factory Rotational Symmetry 1
		0x00081, // Glass Factory Rotational Symmetry 2
		0x00083, // Glass Factory Rotational Symmetry 3
		0x00084, // Glass Factory Melting 1
		0x00082, // Glass Factory Melting 2
		0x0343A, // Glass Factory Melting 3

		0x000B0, // Symmetry Island Door 1
		0x00022, // Symmetry Island Black Dots 1
		0x00023, // Symmetry Island Black Dots 2
		0x00024, // Symmetry Island Black Dots 3
		0x00025, // Symmetry Island Black Dots 4
		0x00026, // Symmetry Island Black Dots 5
		0x0007C, // Symmetry Island Colored Dots 1
		0x0007E, // Symmetry Island Colored Dots 2
		0x00075, // Symmetry Island Colored Dots 3
		0x00073, // Symmetry Island Colored Dots 4
		0x00077, // Symmetry Island Colored Dots 5
		0x00079, // Symmetry Island Colored Dots 6
		0x00065, // Symmetry Island Fading Lines 1
		0x0006D, // Symmetry Island Fading Lines 2
		0x00072, // Symmetry Island Fading Lines 3
		0x0006F, // Symmetry Island Fading Lines 4
		0x00070, // Symmetry Island Fading Lines 5
		0x00071, // Symmetry Island Fading Lines 6
		0x00076, // Symmetry Island Fading Lines 7
		// 0x009B8, // Symmetry Island Transparent 1 // Too mean for right now
		// 0x003E8, // Symmetry Island Transparent 2 // Too mean for right now
		// 0x00A15, // Symmetry Island Transparent 3 // Too mean for right now
		// 0x00B53, // Symmetry Island Transparent 4 // Too mean for right now
		// 0x00B8D, // Symmetry Island Transparent 5 // Too mean for right now
		// 0x1C349, // Symmetry Island Door 2 - Collision fails here, sadly
		0x00A52, // Symmetry Island Laser Yellow 1
		0x00A57, // Symmetry Island Laser Yellow 2
		0x00A5B, // Symmetry Island Laser Yellow 3
		0x00A61, // Symmetry Island Laser Blue 1
		0x00A64, // Symmetry Island Laser Blue 2
		0x00A68, // Symmetry Island Laser Blue 3
		// 0x0360D, // Symmetry Island Laser
	};
	randomizer.Randomize(symmetryPanels, SWAP_PATHWAYS | SWAP_COLORS);



	std::vector<int> desertPanels = {
//		0x17CE7, // Desert Discard
//		0x0CC7B, // Desert Vault
//		0x0339E, // Desert Vault Box
		0x00698, // Desert Surface 1
		0x0048F, // Desert Surface 2
		0x09F92, // Desert Surface 3
//		0x09FA0, // Desert Surface 3 Control
		0x0A036, // Desert Surface 4
		0x09DA6, // Desert Surface 5
		0x0A049, // Desert Surface 6
		0x0A053, // Desert Surface 7
		0x09F94, // Desert Surface 8
//		0x09F86, // Desert Surface 8 Control
//		0x0C339, // Desert Surface Door
//		0x09FAA, // Desert Lightswitch
		0x00422, // Desert Light 1
		0x006E3, // Desert Light 2
//		0x0A02D, // Desert Light 3
		0x00C72, // Desert Pond 1
		0x0129D, // Desert Pond 2
		0x008BB, // Desert Pond 3
		0x0078D, // Desert Pond 4
		0x18313, // Desert Pond 5
//		0x0A249, // Desert Pond Exit Door
//		0x1C2DF, // Desert Flood Control Lower Far Left
//		0x1831E, // Desert Flood Control Lower Far Right
//		0x1C260, // Desert Flood Control Lower Near Left
//		0x1831C, // Desert Flood Control Lower Near Right
//		0x1C2F3, // Desert Flood Control Raise Far Left
//		0x1831D, // Desert Flood Control Raise Far Right
//		0x1C2B1, // Desert Flood Control Raise Near Left
//		0x1831B, // Desert Flood Control Raise Near Right
		0x04D18, // Desert Flood 1
		0x01205, // Desert Flood 2
		0x181AB, // Desert Flood 3
		0x0117A, // Desert Flood 4
		0x17ECA, // Desert Flood 5
//		0x18076, // Desert Flood Exit
//		0x0A15C, // Desert Final Left Convex
//		0x09FFF, // Desert Final Left Concave
//		0x0A15F, // Desert Final Near
//		0x17C31, // Desert Final Transparent
		0x012D7, // Desert Final Far
//		0x0A015, // Desert Final Far Control
//		0x03608, // Desert Laser
	};

	randomizer.Randomize(desertPanels, SWAP_PATHWAYS);

	std::vector<int> quarryPanels = {
		0x01E5A, // Mill Entry Door Left
		0x01E59, // Mill Entry Door Right
//		0x03678, // Mill Lower Ramp Contol
		0x00E0C, // Mill Lower Row 1
		0x01489, // Mill Lower Row 2
		0x0148A, // Mill Lower Row 3
		0x014D9, // Mill Lower Row 4
		0x014E7, // Mill Lower Row 5
		0x014E8, // Mill Lower Row 6
//		0x03679, // Mill Lower Lift Control
		0x00557, // Mill Upper Row 1
		0x005F1, // Mill Upper Row 2
		0x00620, // Mill Upper Row 3
		0x009F5, // Mill Upper Row 4
		0x0146C, // Mill Upper Row 5
		0x3C12D, // Mill Upper Row 6
		0x03686, // Mill Upper Row 7
		0x014E9, // Mill Upper Row 8
		0x0367C, // Mill Control Room 1
//		0x03675, // Mill Upper Lift Control
//		0x03676, // Mill Upper Ramp Control
		0x3C125, // Mill Control Room 2
//		0x275ED, // Mill EP Door
//		0x17CAC, // Mill Stairs Shortcut Door
		0x03677, // Mill Stairs Control
//		0x17CF0, // Mill Discard
//		0x17CC4, // Mill Elevator Control
		0x021D5, // Boathouse Ramp Activation Shapers
		0x034D4, // Boathouse Ramp Activation Stars
//		0x03852, // Boathouse Ramp Angle Control
		0x021B3, // Boathouse Erasers and Shapers 1
		0x021B4, // Boathouse Erasers and Shapers 2
		0x021B0, // Boathouse Erasers and Shapers 3
		0x021AF, // Boathouse Erasers and Shapers 4
		0x021AE, // Boathouse Erasers and Shapers 5
//		0x17CA6, // Boathouse Summon Boat
//		0x03858, // Boathouse Ramp Position Control
//		0x38663, // Boathouse Shortcut
		0x021B5, // Boathouse Erasers and Stars 1
		0x021B6, // Boathouse Erasers and Stars 2
		0x021B7, // Boathouse Erasers and Stars 3
		0x021BB, // Boathouse Erasers and Stars 4
		0x09DB5, // Boathouse Erasers and Stars 5
		0x09DB1, // Boathouse Erasers and Stars 6
		0x3C124, // Boathouse Erasers and Stars 7
		0x09DB3, // Boathouse Erasers Shapers and Stars 1
		0x09DB4, // Boathouse Erasers Shapers and Stars 2
		0x0A3CB, // Boathouse Erasers Shapers and Stars 3
		0x0A3CC, // Boathouse Erasers Shapers and Stars 4
		0x0A3D0, // Boathouse Erasers Shapers and Stars 5
//		0x275FA, // Boathouse Hook Control
		0x09E57, // Quarry Entry Gate 1
		0x17C09, // Quarry Entry Gate 2
//		0x03612, // Quarry Laser
	};
	randomizer.Randomize(quarryPanels, SWAP_PATHWAYS | SWAP_COLORS);


	/*/


	int MILL_L_1 = 0xE0C;
	int MILL_U_1 = 0x557;
	int QUARRY_E_1 = 0x9E57;
	int QUARRY_E_2 = 0x17C09;
	int MILL_E_1 = 0x1E5A;

	randomizer.SwapPanels(QUARRY_E_2, MILL_E_1, SWAP_PATHWAYS | SWAP_COLORS);
	//*/
}

WitnessRandomizer::WitnessRandomizer() : _memory(Memory("witness64_d3d11.exe"))
{
	// Turn off desert flood final
	_memory.WriteData<float>({0x5B28C0, 0x18, 0x18076*8, 0x2A8}, {0.0f});
	// Change desert floating target to desert flood final
	_memory.WriteData<int>({0x5B28C0, 0x18, 0x17ECA*8, 0x2BC}, {0x18077});
}

void WitnessRandomizer::Randomize(std::vector<int> panels, int flags) {
	for (size_t i=panels.size() - 1; i > 1; i--) {
		int target = rand() % i;
		if (i != target) {
			std::cout << "Swapping panels " << std::hex << panels[i] << " and " << std::hex << panels[target] << std::endl;
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
	if (flags & SWAP_PATHWAYS) {
		offsets[0x3B8] = sizeof(int); // num_dots
		offsets[0x3BC] = sizeof(int); // num_connections
		offsets[0x3C8] = sizeof(void*); // *dot_positions
		offsets[0x3D0] = sizeof(void*); // *dot_flags
		offsets[0x3D8] = sizeof(void*); // *dot_connection_a
		offsets[0x3E0] = sizeof(void*); // *dot_connection_b
		offsets[0x440] = sizeof(void*); // *reflection_data
		offsets[0x448] = sizeof(int); // grid_size_x
		offsets[0x44C] = sizeof(int); // grid_size_y
		offsets[0x450] = sizeof(int); // style_flags // This is required to not ignore dots
		offsets[0x4B0] = sizeof(void*); // *panel_target
		offsets[0x4D8] = sizeof(void*); // *specular_texture

		offsets[0x420] = sizeof(void*); // *decorations
		offsets[0x428] = sizeof(void*); // *decoration_flags
		offsets[0x438] = sizeof(int); // num_decorations
									  
		offsets[0x45C] = sizeof(int); // sequence_len
		offsets[0x460] = sizeof(void*); // *sequence
		offsets[0x468] = sizeof(int); // dot_sequence_len
		offsets[0x470] = sizeof(void*); // *dot_sequence
		offsets[0x478] = sizeof(int); // dot_sequence_len_reflection
		offsets[0x480] = sizeof(void*); // *dot_sequence_reflection

		offsets[0x3A4] = sizeof(float); // path_width_scale
		offsets[0x3A8] = sizeof(float); // startpoint_scale

		
		// TODO: Try using is_cylinder to swap into tutorial pillar. If it fails, discard.
		// Probably not: Extra back distance
		// SwapPanelData(panels[i], panels[target], 0x2FC, 125); // is_cylinder through max_connections
	}
	if (flags & SWAP_STYLE) {
		// 340 - pattern_scale
		// 288 - flash_mode
	}
	if (flags & SWAP_COLORS) {
		offsets[0xC8] = 16; // path_color
		offsets[0xD8] = 16; // reflection_path_color
//		offsets[0xE8] = 16; // deprecated_finished_path_color
		offsets[0xF8] = 16; // dot_color
		offsets[0x108] = 16; // active_color
		offsets[0x118] = 16; // background_region_color
		offsets[0x128] = 16; // success_color_a
		offsets[0x138] = 16; // success_color_b
		offsets[0x148] = 16; // strobe_color_a
		offsets[0x158] = 16; // strobe_color_b
		offsets[0x168] = 16; // error_color
//		offsets[0x178] = 16; // video_status_color
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
		offsets[0x4A8] = sizeof(void*); // *colored_regions
	}
	if (flags & SWAP_TRACED) {
		offsets[0x230] = 16; // traced_edges
	}


	for (auto const& [offset, size] : offsets) {
		SwapPanelData(panel1, panel2, offset, size);
	}
}

void WitnessRandomizer::SwapPanelData(int panel1, int panel2, int finalOffset, int dataSize) {
	// Currently wired for old version
	std::vector<int> panel1Offset = {0x5B28C0, 0x18, panel1*8, finalOffset};
	std::vector<int> panel2Offset = {0x5B28C0, 0x18, panel2*8, finalOffset};

	std::vector<byte> panel1Data = _memory.ReadData<byte>(panel1Offset, dataSize);
	std::vector<byte> panel2Data = _memory.ReadData<byte>(panel2Offset, dataSize);

	bool failed = _memory.WriteData<byte>(panel2Offset, panel1Data);
	if (failed) {
		_memory.WriteData<byte>(panel2Offset, panel1Data);
	}
	failed = _memory.WriteData<byte>(panel1Offset, panel2Data);
	if (failed) {
		_memory.WriteData<byte>(panel1Offset, panel2Data);
	}
}
